//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#ifndef forward_subpass_hpp
#define forward_subpass_hpp

#include "rendering/subpass.h"

namespace vox {
class ForwardSubpass : public Subpass {
public:
    ForwardSubpass(RenderContext *context,
                   Scene *scene,
                   Camera *camera);

    void prepare() override;

    void draw(MTL::RenderCommandEncoder &commandEncoder) override;

private:
    void _drawMeshes(MTL::RenderCommandEncoder &renderEncoder);

    void _drawElement(MTL::RenderCommandEncoder &renderEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection &compileMacros);

    std::shared_ptr<MTL::RenderPipelineDescriptor> _forwardPipelineDescriptor;
};

}// namespace vox

#endif /* forward_subpass_hpp */
