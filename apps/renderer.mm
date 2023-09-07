//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "renderer.h"
#include <simd/simd.h>

#include <pxr/pxr.h>
#include <pxr/base/gf/camera.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/plug/plugin.h>
#include "pxr/base/plug/registry.h"
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/bboxCache.h>
#include "pxr/usd/usdGeom/camera.h"
#include "pxr/usd/usdGeom/metrics.h"
#include <pxr/imaging/hgiMetal/hgi.h>
#include <pxr/imaging/hgiMetal/texture.h>
#include <pxr/imaging/hdx/types.h>
#include <pxr/imaging/hgi/blitCmdsOps.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>

#include <pxr/imaging/hdx/tokens.h>

#import <CoreImage/CIContext.h>
#import <MetalKit/MetalKit.h>

#include <cmath>
#include <mutex>
#include <string>
#include <vector>

namespace vox {
Renderer::Renderer(MTL::Device *pDevice)
    : _pDevice(pDevice->retain()) {
    _pCommandQueue = _pDevice->newCommandQueue();
    buildShaders();
    buildBuffers();
}

Renderer::~Renderer() {
    _pVertexPositionsBuffer->release();
    _pVertexColorsBuffer->release();
    _pPSO->release();
    _pCommandQueue->release();
    _pDevice->release();
}

void Renderer::buildShaders() {
    using NS::StringEncoding::UTF8StringEncoding;

    const char *shaderSrc = R"(
       #include <metal_stdlib>
       using namespace metal;

       struct v2f
       {
           float4 position [[position]];
           half3 color;
       };

       v2f vertex vertexMain( uint vertexId [[vertex_id]],
                              device const float3* positions [[buffer(0)]],
                              device const float3* colors [[buffer(1)]] )
       {
           v2f o;
           o.position = float4( positions[ vertexId ], 1.0 );
           o.color = half3 ( colors[ vertexId ] );
           return o;
       }

       half4 fragment fragmentMain( v2f in [[stage_in]] )
       {
           return half4( in.color, 1.0 );
       }
   )";

    NS::Error *pError = nullptr;
    MTL::Library *pLibrary = _pDevice->newLibrary(NS::String::string(shaderSrc, UTF8StringEncoding), nullptr, &pError);
    if (!pLibrary) {
        __builtin_printf("%s", pError->localizedDescription()->utf8String());
        assert(false);
    }

    MTL::Function *pVertexFn = pLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function *pFragFn = pLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

    MTL::RenderPipelineDescriptor *pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction(pVertexFn);
    pDesc->setFragmentFunction(pFragFn);
    pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    _pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
    if (!_pPSO) {
        __builtin_printf("%s", pError->localizedDescription()->utf8String());
        assert(false);
    }

    pVertexFn->release();
    pFragFn->release();
    pDesc->release();
    pLibrary->release();
}

void Renderer::buildBuffers() {
    const size_t NumVertices = 3;

    simd::float3 positions[NumVertices] =
        {
            {-0.8f, 0.8f, 0.0f},
            {0.0f, -0.8f, 0.0f},
            {+0.8f, 0.8f, 0.0f}};

    simd::float3 colors[NumVertices] =
        {
            {1.0, 0.3f, 0.2f},
            {0.8f, 1.0, 0.0f},
            {0.8f, 0.0f, 1.0}};

    const size_t positionsDataSize = NumVertices * sizeof(simd::float3);
    const size_t colorDataSize = NumVertices * sizeof(simd::float3);

    MTL::Buffer *pVertexPositionsBuffer = _pDevice->newBuffer(positionsDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer *pVertexColorsBuffer = _pDevice->newBuffer(colorDataSize, MTL::ResourceStorageModeManaged);

    _pVertexPositionsBuffer = pVertexPositionsBuffer;
    _pVertexColorsBuffer = pVertexColorsBuffer;

    memcpy(_pVertexPositionsBuffer->contents(), positions, positionsDataSize);
    memcpy(_pVertexColorsBuffer->contents(), colors, colorDataSize);

    _pVertexPositionsBuffer->didModifyRange(NS::Range::Make(0, _pVertexPositionsBuffer->length()));
    _pVertexColorsBuffer->didModifyRange(NS::Range::Make(0, _pVertexColorsBuffer->length()));
}

void Renderer::draw(MTK::View *pView) {
    NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer *pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor *pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder *pEnc = pCmd->renderCommandEncoder(pRpd);

    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBuffer(_pVertexColorsBuffer, 0, 1);
    pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

    pEnc->endEncoding();
    pCmd->presentDrawable(pView->currentDrawable());
    pCmd->commit();

    pPool->release();
}

}// namespace vox