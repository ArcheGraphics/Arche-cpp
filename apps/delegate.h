//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

namespace vox {
class Renderer;

class MyMTKViewDelegate : public MTK::ViewDelegate {
public:
    explicit MyMTKViewDelegate(MTK::View *view);
    ~MyMTKViewDelegate() override;

    void drawInMTKView(MTK::View *pView) override;

    void drawableSizeWillChange(MTK::View *pView, CGSize size) override;

    Renderer *get_app() { return _pRenderer.get(); }

private:
    std::unique_ptr<Renderer> _pRenderer;
};

}// namespace vox