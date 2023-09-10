//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "file_format.h"

#include <pxr/base/arch/fileSystem.h>
#include <pxr/base/tf/registryManager.h>
#include <pxr/base/tf/envSetting.h>
#include <pxr/usd/usd/usdcFileFormat.h>

#include <filesystem>

#include "cgltf_util.h"
#include "converter.h"
#include "debug_codes.h"

using namespace vox;
namespace fs = std::filesystem;

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(
    UsdGlTFFileFormatTokens,
    USDGLTF_FILE_FORMAT_TOKENS);

TF_REGISTRY_FUNCTION(TfType) {
    SDF_DEFINE_FILE_FORMAT(UsdGlTFFileFormat, SdfFileFormat);
}

TF_DEFINE_ENV_SETTING(USDGLTF_ENABLE_MTLX, false,
                      "Whether the UsdGlTF Sdf plugin emits MaterialX materials or not.")

// glTF files can contain embedded images. In order to support them in our Sdf file
// format plugin, we create a temporary directory for each glTF file, write the images
// to it, and reference them. Afterwards, this directory gets deleted, however I was
// unable to use the Sdf file format destructor for this purpose, as it does not seem
// to get called. Instead, we instantiate an object with a static lifetime.
class UsdGlTFTmpDirHolder {
private:
    std::vector<std::string> m_dirPaths;

public:
    std::string makeDir() {
        std::string dir = ArchMakeTmpSubdir(ArchGetTmpDir(), "usdGlTF");
        TF_DEBUG(GUC).Msg("created temp dir %s\n", dir.c_str());
        m_dirPaths.push_back(dir);
        return dir;
    }
    ~UsdGlTFTmpDirHolder() {
        for (const std::string &dir : m_dirPaths) {
            TF_DEBUG(GUC).Msg("deleting temp dir %s\n", dir.c_str());
            fs::remove_all(fs::path(dir));
        }
    }
};

static UsdGlTFTmpDirHolder s_tmpDirHolder;

UsdGlTFFileFormat::UsdGlTFFileFormat()
    : SdfFileFormat(
          UsdGlTFFileFormatTokens->Id,
          UsdGlTFFileFormatTokens->Version,
          UsdGlTFFileFormatTokens->Target,
          UsdGlTFFileFormatTokens->Id) {
}

UsdGlTFFileFormat::~UsdGlTFFileFormat() {
}

bool UsdGlTFFileFormat::CanRead(const std::string &filePath) const {
    // FIXME: implement? In my tests, this is not even called.
    return false;
}

bool UsdGlTFFileFormat::Read(SdfLayer *layer,
                             const std::string &resolvedPath,
                             bool metadataOnly) const {
    cgltf_data *gltf_data = nullptr;
    if (!load_gltf(resolvedPath.c_str(), &gltf_data)) {
        TF_RUNTIME_ERROR("unable to load glTF file %s", resolvedPath.c_str());
        return false;
    }

    Converter::Params params = {};
    params.srcDir = fs::path(resolvedPath).parent_path();
    params.dstDir = s_tmpDirHolder.makeDir();
    params.mtlxFileName = "";// Not needed because of Mtlx-as-UsdShade option
    params.copyExistingFiles = false;
    params.genRelativePaths = false;
    params.emitMtlx = TfGetEnvSetting(USDGLTF_ENABLE_MTLX);
    params.mtlxAsUsdShade = true;
    params.explicitColorspaceTransforms = false;
    params.gltfPbrImpl = Converter::GltfPbrImpl::Runtime;
    params.hdStormCompat = false;
    params.defaultMaterialVariant = 0;

    SdfLayerRefPtr tmpLayer = SdfLayer::CreateAnonymous(".usdc");
    UsdStageRefPtr stage = UsdStage::Open(tmpLayer);

    Converter converter(gltf_data, stage, params);

    Converter::FileExports fileExports;// only used for USDZ
    converter.convert(fileExports);

    cgltf_free(gltf_data);

    layer->TransferContent(tmpLayer);

    return true;
}

bool UsdGlTFFileFormat::ReadFromString(SdfLayer *layer,
                                       const std::string &str) const {
    // glTF files often reference other files (e.g. a .bin payload or images).
    // Hence, without a file location, most glTF files can not be loaded correctly.
    return false;
}

bool UsdGlTFFileFormat::WriteToString(const SdfLayer &layer,
                                      std::string *str,
                                      const std::string &comment) const {
    // Not supported, and never will be. Write USDC instead.
    SdfFileFormatConstPtr usdcFormat = SdfFileFormat::FindById(UsdUsdcFileFormatTokens->Id);
    return usdcFormat->WriteToString(layer, str, comment);
}

bool UsdGlTFFileFormat::WriteToStream(const SdfSpecHandle &spec,
                                      std::ostream &out,
                                      size_t indent) const {
    // Not supported, and never will be. Write USDC instead.
    SdfFileFormatConstPtr usdcFormat = SdfFileFormat::FindById(UsdUsdcFileFormatTokens->Id);
    return usdcFormat->WriteToStream(spec, out, indent);
}

PXR_NAMESPACE_CLOSE_SCOPE
