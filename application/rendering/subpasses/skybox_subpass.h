//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef skybox_subpass_hpp
#define skybox_subpass_hpp

#include "rendering/subpass.h"
#include "graphics/mesh.h"
#include "texture/sampled_texturecube.h"

namespace vox {
class SkyboxSubpass : public Subpass {
public:
    SkyboxSubpass(RenderContext *context,
                  Scene *scene,
                  Camera *camera);

    void prepare() override;

    void draw(MTL::RenderCommandEncoder &commandEncoder) override;

public:
    void createSphere(float radius);

    void createCuboid();

    /**
     * Texture cube map of the sky box material.
     */
    const SampledTextureCubePtr &textureCubeMap() const;

    void setTextureCubeMap(const SampledTextureCubePtr &v);

private:
    std::shared_ptr<MTL::RenderPipelineDescriptor> _skyboxPipelineDescriptor;
    std::shared_ptr<MTL::DepthStencilState> _dontWriteDepthStencilState;

    enum class SkyBoxType {
        Cuboid,
        Sphere
    };
    SkyBoxType _type;
    std::shared_ptr<Mesh> _mesh{nullptr};
    SampledTextureCubePtr _cubeMap{nullptr};
};

}// namespace vox

#endif /* skybox_subpass_hpp */
