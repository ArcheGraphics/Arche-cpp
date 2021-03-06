//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef forward_subpass_hpp
#define forward_subpass_hpp

#include "rendering/subpass.h"

namespace vox {
class ForwardSubpass: public Subpass {
public:
    enum class RenderMode {
        AUTO,
        MANUAL
    };
    
    ForwardSubpass(RenderContext* renderContext,
                   wgpu::TextureFormat depthStencilTextureFormat,
                   Scene* scene,
                   Camera* camera);
    
    void prepare() override;
    
    void draw(wgpu::RenderPassEncoder& passEncoder) override;
    
public:
    RenderMode renderMode() const;
    
    void setRenderMode(RenderMode mode);
    
    void addRenderElement(const RenderElement& element);
    
    void clearAllRenderElement();
    
private:
    void _drawMeshes(wgpu::RenderPassEncoder &passEncoder);
    
    void _drawElement(wgpu::RenderPassEncoder &passEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection& compileMacros);
    
    void _bindingData(wgpu::BindGroupEntry& entry,
                      MaterialPtr mat, Renderer* renderer);
    
    void _bindingTexture(wgpu::BindGroupEntry& entry,
                         MaterialPtr mat, Renderer* renderer);
    
    void _bindingSampler(wgpu::BindGroupEntry& entry,
                         MaterialPtr mat, Renderer* renderer);
    
    wgpu::RenderPipelineDescriptor _forwardPipelineDescriptor;
    wgpu::DepthStencilState _depthStencil;
    wgpu::FragmentState _fragment;
    wgpu::ColorTargetState _colorTargetState;
    
    wgpu::BindGroupDescriptor _bindGroupDescriptor;
    std::vector<wgpu::BindGroupEntry> _bindGroupEntries{};
    
    wgpu::PipelineLayoutDescriptor _pipelineLayoutDescriptor;
    wgpu::PipelineLayout _pipelineLayout;
    
    wgpu::TextureFormat _depthStencilTextureFormat;
    
    RenderMode _mode = RenderMode::AUTO;
    std::vector<RenderElement> _elements{};
};

}

#endif /* forward_subpass_hpp */
