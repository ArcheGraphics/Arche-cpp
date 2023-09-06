//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture_utils.h"
#include "metal_helpers.h"
#include <glog/logging.h>

namespace vox {
void TextureUtils::buildTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                     const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                     uint32_t width, uint32_t height,
                                     MTL::Texture &textureArray,
                                     MTL::CommandBuffer &commandBuffer) {
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer.blitCommandEncoder());
    MTL::Origin origin = MTL::Origin(0, 0, 0);
    MTL::Size size = MTL::Size(textureArray.width(), textureArray.height(), 1);

    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        blit->copyFromTexture(iter->get(), 0, 0, origin, size,
                              &textureArray, iter - texturesBegin, 0, origin);
    }
    blit->endEncoding();
}

void TextureUtils::buildCubeTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                         const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                         uint32_t width, uint32_t height,
                                         MTL::Texture &textureArray,
                                         MTL::CommandBuffer &commandBuffer) {
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer.blitCommandEncoder());
    int destinationSlice = 0;
    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        blit->copyFromTexture(iter->get(), 0, 0, &textureArray, destinationSlice, 0, 6, 1);
        destinationSlice += 6;
    }
    blit->endEncoding();
}

//MARK: - PBR
std::shared_ptr<SampledTexture2D>
TextureUtils::createMetallicRoughnessTexture(const MTL::Texture &metallic, const MTL::Texture &roughness,
                                             MTL::Device &device, MTL::Library &library, MTL::CommandQueue &queue) {
    auto mergedTexture = std::make_shared<SampledTexture2D>(device,
                                                            metallic.width(), metallic.height(), 1, false,
                                                            metallic.pixelFormat(),
                                                            metallic.usage() | MTL::TextureUsageShaderWrite);

    auto brdfFunction =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   library.newFunction(NS::String::string("build_metallicRoughness",
                                                                          NS::StringEncoding::UTF8StringEncoding)));
    NS::Error *error{nullptr};
    auto pipelineState =
        CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState, device.newComputePipelineState(brdfFunction.get(), &error));
    if (error != nullptr) {
        LOG(ERROR) << "Error: failed to create Metal pipeline state: "
                   << error->description()->cString(NS::StringEncoding::UTF8StringEncoding);
    }
    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue.commandBuffer());
    auto commandEncoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder, commandBuffer->computeCommandEncoder());

    commandEncoder->setComputePipelineState(pipelineState.get());
    commandEncoder->setTexture(&metallic, 0);
    commandEncoder->setTexture(&roughness, 1);
    commandEncoder->setTexture(&mergedTexture->texture(), 2);

    auto size = metallic.width();
    auto threadsPerThreadgroup = MTL::Size(std::min<size_t>(size, 16), std::min<size_t>(size, 16), 1);
    auto threadgroups = MTL::Size(metallic.width() / threadsPerThreadgroup.width,
                                  metallic.height() / threadsPerThreadgroup.height, 1);
    commandEncoder->dispatchThreadgroups(threadgroups, threadsPerThreadgroup);
    commandEncoder->endEncoding();

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    return mergedTexture;
}

std::shared_ptr<SampledTexture2D>
TextureUtils::createBRDFLookupTable(MTL::Device &device, MTL::Library &library, MTL::CommandQueue &queue) {
    auto brdfFunction =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   library.newFunction(NS::String::string("integrateBRDF",
                                                                          NS::StringEncoding::UTF8StringEncoding)));
    NS::Error *error{nullptr};
    auto brdfPipelineState =
        CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState, device.newComputePipelineState(brdfFunction.get(), &error));
    if (error != nullptr) {
        LOG(ERROR) << "Error: failed to create Metal pipeline state: "
                   << error->description()->cString(NS::StringEncoding::UTF8StringEncoding);
    }
    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue.commandBuffer());
    auto commandEncoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder, commandBuffer->computeCommandEncoder());

    const uint32_t size = 256;
    auto lut = std::make_shared<SampledTexture2D>(device,
                                                  size, size, 1, false,
                                                  MTL::PixelFormatRG16Float,
                                                  MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);
    commandEncoder->setComputePipelineState(brdfPipelineState.get());
    commandEncoder->setTexture(&lut->texture(), 0);
    auto threadsPerThreadgroup = MTL::Size(16, 16, 1);
    auto threadgroups = MTL::Size(size / threadsPerThreadgroup.width,
                                  size / threadsPerThreadgroup.height, 1);
    commandEncoder->dispatchThreadgroups(threadgroups, threadsPerThreadgroup);
    commandEncoder->endEncoding();

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    return lut;
}

std::shared_ptr<SampledTextureCube>
TextureUtils::createSpecularTexture(const MTL::Texture &cube,
                                    MTL::Device &device, MTL::Library &library, MTL::CommandQueue &queue) {
    auto specularTexture = std::make_shared<SampledTextureCube>(device,
                                                                cube.width(), cube.height(), 1, true,
                                                                cube.pixelFormat(),
                                                                cube.usage() | MTL::TextureUsageShaderRead);

    auto function =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   library.newFunction(NS::String::string("build_specular",
                                                                          NS::StringEncoding::UTF8StringEncoding)));
    NS::Error *error{nullptr};
    auto pipelineState =
        CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState, device.newComputePipelineState(function.get(), &error));
    if (error != nullptr) {
        LOG(ERROR) << "Error: failed to create Metal pipeline state: "
                   << error->description()->cString(NS::StringEncoding::UTF8StringEncoding);
    }
    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, queue.commandBuffer());

    auto blitEncoder = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
    blitEncoder->copyFromTexture(&cube, 0, 0, &specularTexture->texture(), 0, 0, 6, 1);
    blitEncoder->endEncoding();

    // generate Mipmap
    for (int level = 1; level < 9; level++) {
        LOG(INFO) << "Processing level: " << level << std::endl;
        auto size = cube.width() / int(pow(2, float(level)));
        auto descriptor = MTL::TextureDescriptor::textureCubeDescriptor(MTL::PixelFormatRGBA8Unorm, size, false);
        descriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);
        auto outputTexture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, device.newTexture(descriptor));

        auto commandEncoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder, commandBuffer->computeCommandEncoder());
        commandEncoder->setComputePipelineState(pipelineState.get());
        commandEncoder->setTexture(&cube, 0);
        commandEncoder->setTexture(outputTexture.get(), 1);

        float roughness = float(level) / 10;
        commandEncoder->setBytes(&roughness, sizeof(float), 0);
        auto threadsPerThreadgroup = MTL::Size(std::min<size_t>(size, 16), std::min<size_t>(size, 16), 1);
        auto threadgroups = MTL::Size(cube.width() / threadsPerThreadgroup.width,
                                      cube.width() / threadsPerThreadgroup.height, 6);
        commandEncoder->dispatchThreadgroups(threadgroups, threadsPerThreadgroup);
        commandEncoder->endEncoding();

        auto blitEncoder = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
        blitEncoder->copyFromTexture(outputTexture.get(), 0, 0, &specularTexture->texture(), 0, level, 6, 1);
        blitEncoder->endEncoding();
    }

    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    return specularTexture;
}

}// namespace vox
