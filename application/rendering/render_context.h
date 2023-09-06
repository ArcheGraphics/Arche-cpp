//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_context_hpp
#define render_context_hpp

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

struct GLFWwindow;

namespace vox {
class RenderContextImpl;

class RenderContext {
public:
    RenderContext(MTL::Device &device, GLFWwindow *window,
                  uint32_t width, uint32_t height);

    ~RenderContext();

    MTL::Device &device();

    void resize(uint32_t width, uint32_t height);

    void nextDrawable();

    MTL::Texture *currentDrawableTexture();

    MTL::PixelFormat drawableTextureFormat();

    MTL::Texture *depthStencilTexture();

    MTL::PixelFormat depthStencilTextureFormat();

    CA::MetalDrawable *currentDrawable();

private:
    MTL::Device &_device;

    CA::MetalDrawable *_currentDrawable{nullptr};
    MTL::Texture *_currentTexture{nullptr};
    MTL::PixelFormat _colorTextureFormat = MTL::PixelFormatBGRA8Unorm_sRGB;
    MTL::Texture *_depthStencilTexture{nullptr};
    MTL::PixelFormat _depthStencilTextureFormat = MTL::PixelFormatDepth32Float_Stencil8;

    std::unique_ptr<RenderContextImpl> _impl{nullptr};
};

}// namespace vox

#endif /* render_context_hpp */
