//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "color_picker_subpass.h"
#include "rendering/render_pass.h"
#include "material/material.h"
#include "graphics/mesh.h"
#include "renderer.h"
#include "camera.h"
#include "metal_helpers.h"

namespace vox {
ColorPickerSubpass::ColorPickerSubpass(RenderContext *context,
                                       Scene *scene,
                                       Camera *camera) : Subpass(context, scene, camera) {
}

void ColorPickerSubpass::prepare() {
    _forwardPipelineDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPipelineDescriptor, MTL::RenderPipelineDescriptor::alloc()->init());
    _forwardPipelineDescriptor->setLabel(NS::String::string("ColorPicker Pipeline", NS::StringEncoding::UTF8StringEncoding));
    _forwardPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(_pass->renderPassDescriptor().colorAttachments()->object(0)->texture()->pixelFormat());
    _forwardPipelineDescriptor->setDepthAttachmentPixelFormat(_context->depthStencilTextureFormat());

    std::shared_ptr<MTL::DepthStencilDescriptor> depthStencilDesc = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilDescriptor,
                                                                                               MTL::DepthStencilDescriptor::alloc()->init());
    depthStencilDesc->setLabel(NS::String::string("ColorPicker Creation", NS::StringEncoding::UTF8StringEncoding));
    depthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
    depthStencilDesc->setDepthWriteEnabled(true);
    _forwardDepthStencilState = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilState,
                                                           _context->device().newDepthStencilState(depthStencilDesc.get()));
}

void ColorPickerSubpass::draw(MTL::RenderCommandEncoder &commandEncoder) {
    _currentId = 0;
    _primitivesMap.clear();

    commandEncoder.pushDebugGroup(NS::String::string("Draw ColorPicker", NS::StringEncoding::UTF8StringEncoding));
    commandEncoder.setCullMode(MTL::CullModeFront);
    commandEncoder.setDepthStencilState(_forwardDepthStencilState.get());
    commandEncoder.setStencilReferenceValue(128);

    _drawMeshes(commandEncoder);
    commandEncoder.popDebugGroup();
}

void ColorPickerSubpass::_drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    auto compileMacros = ShaderMacroCollection();
    _scene->shaderData.mergeMacro(compileMacros, compileMacros);
    _camera->shaderData.mergeMacro(compileMacros, compileMacros);

    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    _scene->_componentsManager.callRender(_camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    _drawElement(renderEncoder, opaqueQueue, compileMacros);
    _drawElement(renderEncoder, alphaTestQueue, compileMacros);
    _drawElement(renderEncoder, transparentQueue, compileMacros);
}

void ColorPickerSubpass::_drawElement(MTL::RenderCommandEncoder &renderEncoder,
                                      const std::vector<RenderElement> &items,
                                      const ShaderMacroCollection &compileMacros) {
    for (auto &element : items) {
        auto macros = compileMacros;
        auto renderer = element.renderer;
        renderer->updateShaderData(_camera->viewMatrix(), _camera->projectionMatrix());
        renderer->shaderData.mergeMacro(macros, macros);

        auto material = element.material;
        material->shaderData.mergeMacro(macros, macros);

        auto vertexFunction = _pass->resourceCache().requestFunction(_pass->library(), "vertex_picker", macros);
        auto fragmentFunction = _pass->resourceCache().requestFunction(_pass->library(), "fragment_picker", macros);
        _forwardPipelineDescriptor->setVertexFunction(vertexFunction);
        _forwardPipelineDescriptor->setFragmentFunction(fragmentFunction);

        // manully
        auto &mesh = element.mesh;
        _forwardPipelineDescriptor->setVertexDescriptor(mesh->vertexDescriptor().get());

        auto forwardPipelineState = _pass->resourceCache().requestPipelineState(*_forwardPipelineDescriptor);
        uploadUniforms(renderEncoder, forwardPipelineState->materialUniformBlock, material->shaderData);
        uploadUniforms(renderEncoder, forwardPipelineState->rendererUniformBlock, renderer->shaderData);
        uploadUniforms(renderEncoder, forwardPipelineState->sceneUniformBlock, _scene->shaderData);
        uploadUniforms(renderEncoder, forwardPipelineState->cameraUniformBlock, _camera->shaderData);
        renderEncoder.setRenderPipelineState(&forwardPipelineState->handle());

        _currentId += 1;
        _primitivesMap[_currentId] = std::make_pair(renderer, mesh);
        Vector3F color = id2Color(_currentId);
        renderEncoder.setFragmentBytes(&color, sizeof(Vector4F), 0);

        uint32_t index = 0;
        for (auto &meshBuffer : mesh->vertexBufferBindings()) {
            renderEncoder.setVertexBuffer(meshBuffer.get(),
                                          0, index++);
        }
        auto &submesh = element.subMesh;

        if (submesh->indexBuffer()) {
            renderEncoder.drawIndexedPrimitives(submesh->primitiveType(),
                                                submesh->indexCount(),
                                                submesh->indexType(),
                                                submesh->indexBuffer().get(),
                                                0);
        } else {
            renderEncoder.drawPrimitives(submesh->primitiveType(),
                                         NS::UInteger(0),
                                         submesh->indexCount());
        }
    }
}

//MARK: - Picker
Vector3F ColorPickerSubpass::id2Color(uint32_t id) {
    if (id >= 0xffffff) {
        std::cout << "Framebuffer Picker encounter primitive's id greater than " + std::to_string(0xffffff)
                  << std::endl;
        return Vector3F(0, 0, 0);
    }

    return Vector3F((id & 0xff) / 255.0, ((id & 0xff00) >> 8) / 255.0, ((id & 0xff0000) >> 16) / 255.0);
}

uint32_t ColorPickerSubpass::color2Id(const std::array<uint8_t, 4> &color) {
    return color[0] | (color[1] << 8) | (color[2] << 16);
}

std::pair<Renderer *, MeshPtr> ColorPickerSubpass::getObjectByColor(const std::array<uint8_t, 4> &color) {
    auto iter = _primitivesMap.find(color2Id(color));
    if (iter != _primitivesMap.end()) {
        return iter->second;
    } else {
        return std::make_pair(nullptr, nullptr);
    }
}

}// namespace vox
