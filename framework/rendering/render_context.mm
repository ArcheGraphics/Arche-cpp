//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_context.h"
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_COCOA

#include <GLFW/glfw3native.h>
#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/QuartzCore.hpp>

namespace vox {
class RenderContextImpl {
private:
    const MTL::Device &_device;
    GLFWwindow *_window{nullptr};

    CAMetalLayer *_layer{nullptr};
    CA::MetalDrawable *_currentDrawable{nullptr};
    MTL::Texture *_currentTexture{nullptr};

public:
    RenderContextImpl(MTL::Device &device, GLFWwindow *window) : _device(device), _window(window) {
    }

    void configure(MTL::PixelFormat format,
                   MTL::TextureUsage usage,
                   uint32_t width,
                   uint32_t height) {
        NSWindow *nswin = glfwGetCocoaWindow(_window);
        NSView *contentView = [nswin contentView];
        [contentView setWantsLayer:YES];

        CGSize size = {};
        size.width = width;
        size.height = height;

        _layer = [CAMetalLayer layer];
        [_layer setDevice:(__bridge id<MTLDevice>)&_device];
        [_layer setPixelFormat:MTLPixelFormatBGRA8Unorm_sRGB];
        [_layer setDrawableSize:size];

        constexpr uint32_t kFramebufferOnlyTextureUsages = MTL::TextureUsageRenderTarget;
        bool hasOnlyFramebufferUsages = !(usage & (~kFramebufferOnlyTextureUsages));
        if (hasOnlyFramebufferUsages) {
            [_layer setFramebufferOnly:YES];
        }

        [contentView setLayer:_layer];
    }

    CA::MetalDrawable *nextDrawable() {
        _currentDrawable->release();
        _currentDrawable = (__bridge CA::MetalDrawable *)[_layer nextDrawable];
        _currentDrawable->retain();

        _currentTexture->release();
        _currentTexture = _currentDrawable->texture();
        _currentTexture->retain();

        return _currentDrawable;
    }

    MTL::Texture *currentTexture() {
        return _currentTexture;
    }
};

//MARK: - RenderContext
RenderContext::RenderContext(MTL::Device &device, GLFWwindow *window,
                             uint32_t width, uint32_t height) : _device(device) {
    _impl = std::make_unique<RenderContextImpl>(device, window);
    resize(width, height);
}

RenderContext::~RenderContext() {
    _impl.reset();
}

MTL::Device &RenderContext::device() {
    return _device;
}

void RenderContext::nextDrawable() {
    _currentDrawable = _impl->nextDrawable();
    _currentTexture = _impl->currentTexture();
}

void RenderContext::resize(uint32_t width, uint32_t height) {
    _impl->configure(_colorTextureFormat, MTL::TextureUsageRenderTarget, width, height);

    // managed by internal no need to release
    MTL::TextureDescriptor *descriptor = MTL::TextureDescriptor::texture2DDescriptor(_depthStencilTextureFormat,
                                                                                     width, height, false);
    descriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
    descriptor->setStorageMode(MTL::StorageModePrivate);
    _depthStencilTexture = _device.newTexture(descriptor);
}

MTL::Texture *RenderContext::currentDrawableTexture() {
    return _currentTexture;
}

MTL::PixelFormat RenderContext::drawableTextureFormat() {
    return _colorTextureFormat;
}

MTL::Texture *RenderContext::depthStencilTexture() {
    return _depthStencilTexture;
}

MTL::PixelFormat RenderContext::depthStencilTextureFormat() {
    return _depthStencilTextureFormat;
}

CA::MetalDrawable *RenderContext::currentDrawable() {
    return _currentDrawable;
}

}// namespace vox
