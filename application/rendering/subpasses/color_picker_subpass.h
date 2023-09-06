//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef color_picker_subpass_hpp
#define color_picker_subpass_hpp

#include "rendering/subpass.h"

namespace vox {
class ColorPickerSubpass : public Subpass {
public:
    ColorPickerSubpass(RenderContext *context,
                       Scene *scene,
                       Camera *camera);

    void prepare() override;

    void draw(MTL::RenderCommandEncoder &commandEncoder) override;

public:
    /**
     * Convert id to RGB color value, 0 and 0xffffff are illegal values.
     */
    Vector3F id2Color(uint32_t id);

    /**
     * Convert RGB color to id.
     * @param color - Color
     */
    uint32_t color2Id(const std::array<uint8_t, 4> &color);

    /**
     * Get renderer element by color.
     */
    std::pair<Renderer *, MeshPtr> getObjectByColor(const std::array<uint8_t, 4> &color);

private:
    void _drawMeshes(MTL::RenderCommandEncoder &renderEncoder);

    void _drawElement(MTL::RenderCommandEncoder &renderEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection &compileMacros);

    std::shared_ptr<MTL::RenderPipelineDescriptor> _forwardPipelineDescriptor;
    std::shared_ptr<MTL::DepthStencilState> _forwardDepthStencilState;

    uint32_t _currentId = 0;
    std::unordered_map<size_t, std::pair<Renderer *, MeshPtr>> _primitivesMap;
};

}// namespace vox

#endif /* color_picker_subpass_hpp */
