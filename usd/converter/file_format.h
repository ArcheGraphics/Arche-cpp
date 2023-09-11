//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/pxr.h>
#include <pxr/usd/sdf/fileFormat.h>
#include <pxr/base/tf/staticTokens.h>
#include <iosfwd>
#include <string>

PXR_NAMESPACE_OPEN_SCOPE

#define USDGLTF_FILE_FORMAT_TOKENS \
    ((Id, "glTF"))((Version, "1.0"))((Target, "usd"))

TF_DECLARE_PUBLIC_TOKENS(UsdGlTFFileFormatTokens, USDGLTF_FILE_FORMAT_TOKENS);

TF_DECLARE_WEAK_AND_REF_PTRS(UsdGlTFFileFormat);

class UsdGlTFFileFormat : public SdfFileFormat {
public:
    SDF_FILE_FORMAT_FACTORY_ACCESS;

    UsdGlTFFileFormat();

    ~UsdGlTFFileFormat() override;

public:
    bool CanRead(const std::string &file) const override;

    bool Read(SdfLayer *layer,
              const std::string &resolvedPath,
              bool metadataOnly) const override;

    bool ReadFromString(SdfLayer *layer,
                        const std::string &str) const override;

    bool WriteToString(const SdfLayer &layer,
                       std::string *str,
                       const std::string &comment = std::string())
        const override;

    bool WriteToStream(const SdfSpecHandle &spec,
                       std::ostream &out,
                       size_t indent) const override;
};

PXR_NAMESPACE_CLOSE_SCOPE
