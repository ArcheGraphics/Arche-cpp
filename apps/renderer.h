//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

namespace vox {
class Renderer {
public:
    explicit Renderer(MTL::Device *pDevice);
    ~Renderer();
    void buildShaders();
    void buildBuffers();
    void draw(MTK::View *pView);

private:
    MTL::Device *_pDevice;
    MTL::CommandQueue *_pCommandQueue;
    MTL::RenderPipelineState *_pPSO{};
    MTL::Buffer *_pVertexPositionsBuffer{};
    MTL::Buffer *_pVertexColorsBuffer{};
};
}// namespace vox