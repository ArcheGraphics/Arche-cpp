//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_subpass.h"
#include "rendering/render_pass.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "components/renderer.h"
#include "components/camera.h"
#include "framework/common/metal_helpers.h"

namespace vox {
ForwardSubpass::ForwardSubpass(RenderContext *context,
                               Scene *scene,
                               Camera *camera) : Subpass(context, scene, camera) {
}

void ForwardSubpass::prepare() {
    _forwardPipelineDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPipelineDescriptor, MTL::RenderPipelineDescriptor::alloc()->init());
    _forwardPipelineDescriptor->setLabel(NS::String::string("Forward Pipeline", NS::StringEncoding::UTF8StringEncoding));
    _forwardPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(_context->drawableTextureFormat());
    _forwardPipelineDescriptor->setDepthAttachmentPixelFormat(_context->depthStencilTextureFormat());
    _forwardPipelineDescriptor->setStencilAttachmentPixelFormat(_context->depthStencilTextureFormat());
}

void ForwardSubpass::draw(MTL::RenderCommandEncoder &commandEncoder) {
    commandEncoder.pushDebugGroup(NS::String::string("Draw Element", NS::StringEncoding::UTF8StringEncoding));
    _drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void ForwardSubpass::_drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shader_data.merge_macro(compileMacros, compileMacros);
    _camera->shaderData.merge_macro(compileMacros, compileMacros);

    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    //    _scene->_componentsManager.callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    _drawElement(renderEncoder, opaqueQueue, compileMacros);
    _drawElement(renderEncoder, alphaTestQueue, compileMacros);
    _drawElement(renderEncoder, transparentQueue, compileMacros);
}

void ForwardSubpass::_drawElement(MTL::RenderCommandEncoder &renderEncoder,
                                  const std::vector<RenderElement> &items,
                                  const ShaderMacroCollection &compileMacros) {
    for (auto &element : items) {
        auto macros = compileMacros;
        auto renderer = element.renderer;

        renderer->update_shader_data();
        renderer->shader_data_.merge_macro(macros, macros);

        auto material = element.material;
        material->shader_data.merge_macro(macros, macros);

        auto vertexFunction = _pass->resourceCache().requestFunction(_pass->library(), material->vertex_source, macros);
        auto fragmentFunction = _pass->resourceCache().requestFunction(_pass->library(), material->fragment_source, macros);
        _forwardPipelineDescriptor->setVertexFunction(vertexFunction);
        _forwardPipelineDescriptor->setFragmentFunction(fragmentFunction);

        // manully
        auto &mesh = element.mesh;
        _forwardPipelineDescriptor->setVertexDescriptor(mesh->vertexDescriptor().get());

        std::shared_ptr<MTL::DepthStencilDescriptor> depthStencilDesc = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilDescriptor, MTL::DepthStencilDescriptor::alloc()->init());
        material->renderState.apply(*_forwardPipelineDescriptor, *depthStencilDesc, renderEncoder);

        auto _forwardDepthStencilState = _pass->resourceCache().requestDepthStencilState(*depthStencilDesc);
        auto _forwardPipelineState = _pass->resourceCache().requestPipelineState(*_forwardPipelineDescriptor);
        //        uploadUniforms(renderEncoder, _forwardPipelineState->uniformBlock, material->shader_data);
        //        uploadUniforms(renderEncoder, _forwardPipelineState->uniformBlock, renderer->shader_data_);
        //        uploadUniforms(renderEncoder, _forwardPipelineState->uniformBlock, _scene->shader_data);
        //        uploadUniforms(renderEncoder, _forwardPipelineState->uniformBlock, _camera->shaderData);
        renderEncoder.setRenderPipelineState(&_forwardPipelineState->handle());
        renderEncoder.setDepthStencilState(_forwardDepthStencilState);

        uint32_t index = 0;
        for (auto &meshBuffer : mesh->vertexBufferBindings()) {
            renderEncoder.setVertexBuffer(meshBuffer.get(),
                                          0, index++);
        }
        auto &submesh = element.sub_mesh;

        if (submesh->indexBuffer()) {
            renderEncoder.drawIndexedPrimitives(submesh->primitiveType(),
                                                submesh->indexCount(),
                                                submesh->indexType(),
                                                submesh->indexBuffer().get(),
                                                0,
                                                mesh->instanceCount());
        } else {
            renderEncoder.drawPrimitives(submesh->primitiveType(),
                                         NS::UInteger(0),
                                         submesh->indexCount(),
                                         mesh->instanceCount());
        }
    }
}

}// namespace vox
