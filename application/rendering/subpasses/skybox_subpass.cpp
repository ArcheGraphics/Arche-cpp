//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "skybox_subpass.h"
#include "rendering/render_pass.h"
#include "mesh/primitive_mesh.h"
#include "camera.h"
#include "metal_helpers.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(RenderContext *context,
                             Scene *scene,
                             Camera *camera) : Subpass(context, scene, camera) {
}

void SkyboxSubpass::createSphere(float radius) {
    _mesh = PrimitiveMesh::createSphere(_context->device(), radius);
    _type = SkyBoxType::Sphere;
}

void SkyboxSubpass::createCuboid() {
    _mesh = PrimitiveMesh::createCuboid(_context->device(), 1, 1, 1);
    _type = SkyBoxType::Cuboid;
}

const SampledTextureCubePtr &SkyboxSubpass::textureCubeMap() const {
    return _cubeMap;
}

void SkyboxSubpass::setTextureCubeMap(const SampledTextureCubePtr &v) {
    _cubeMap = v;
}

//MARK: - Render
void SkyboxSubpass::prepare() {
#pragma mark Sky render pipeline setup
    _skyboxPipelineDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPipelineDescriptor, MTL::RenderPipelineDescriptor::alloc()->init());
    {
        _skyboxPipelineDescriptor->setLabel(NS::String::string("Sky", NS::StringEncoding::UTF8StringEncoding));
        _skyboxPipelineDescriptor->setVertexDescriptor(_mesh->vertexDescriptor().get());
        _skyboxPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(_context->drawableTextureFormat());
        _skyboxPipelineDescriptor->setDepthAttachmentPixelFormat(_context->depthStencilTextureFormat());
        _skyboxPipelineDescriptor->setStencilAttachmentPixelFormat(_context->depthStencilTextureFormat());

        if (_type == SkyBoxType::Sphere) {
            MTL::Function *skyboxVertexFunction =
                _pass->library().newFunction(NS::String::string("vertex_sphere_skybox",
                                                                NS::StringEncoding::UTF8StringEncoding));
            _skyboxPipelineDescriptor->setVertexFunction(skyboxVertexFunction);
        } else {
            MTL::Function *skyboxVertexFunction =
                _pass->library().newFunction(NS::String::string("vertex_cube_skybox",
                                                                NS::StringEncoding::UTF8StringEncoding));
            _skyboxPipelineDescriptor->setVertexFunction(skyboxVertexFunction);
        }
        MTL::Function *skyboxFragmentFunction =
            _pass->library().newFunction(NS::String::string("fragment_skybox",
                                                            NS::StringEncoding::UTF8StringEncoding));
        _skyboxPipelineDescriptor->setFragmentFunction(skyboxFragmentFunction);
    }

#pragma mark Post lighting depth state setup
    {
        std::shared_ptr<MTL::DepthStencilDescriptor> depthStencilDesc = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilDescriptor,
                                                                                                   MTL::DepthStencilDescriptor::alloc()->init());
        depthStencilDesc->setLabel(NS::String::string("Less -Writes", NS::StringEncoding::UTF8StringEncoding));
        depthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc->setDepthWriteEnabled(false);

        _dontWriteDepthStencilState = CLONE_METAL_CUSTOM_DELETER(MTL::DepthStencilState,
                                                                 _context->device().newDepthStencilState(depthStencilDesc.get()));
    }
}

void SkyboxSubpass::draw(MTL::RenderCommandEncoder &commandEncoder) {
    commandEncoder.pushDebugGroup(NS::String::string("Draw Sky", NS::StringEncoding::UTF8StringEncoding));
    auto _skyboxPipelineState = _pass->resourceCache().requestPipelineState(*_skyboxPipelineDescriptor);
    commandEncoder.setRenderPipelineState(&_skyboxPipelineState->handle());
    commandEncoder.setDepthStencilState(_dontWriteDepthStencilState.get());
    commandEncoder.setCullMode(MTL::CullModeBack);

    const auto projectionMatrix = _camera->projectionMatrix();
    auto viewMatrix = _camera->viewMatrix();
    if (_type == SkyBoxType::Cuboid) {
        viewMatrix[12] = 0;
        viewMatrix[13] = 0;
        viewMatrix[14] = 0;
        viewMatrix[15] = 1;
    }
    auto _matrix = projectionMatrix * viewMatrix;
    commandEncoder.setVertexBytes(_matrix.data(), sizeof(Matrix4x4F), 10);
    commandEncoder.setFragmentTexture(&_cubeMap->texture(), 0);

    uint32_t index = 0;
    for (auto &meshBuffer : _mesh->vertexBufferBindings()) {
        commandEncoder.setVertexBuffer(meshBuffer.get(),
                                       0, index++);
    }
    auto submesh = _mesh->subMesh();
    commandEncoder.drawIndexedPrimitives(submesh->primitiveType(),
                                         submesh->indexCount(),
                                         submesh->indexType(),
                                         submesh->indexBuffer().get(),
                                         0);

    commandEncoder.popDebugGroup();
}

}// namespace vox
