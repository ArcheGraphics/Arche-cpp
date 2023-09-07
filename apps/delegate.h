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
    explicit MyMTKViewDelegate(MTL::Device *pDevice);
    ~MyMTKViewDelegate() override;
    void drawInMTKView(MTK::View *pView) override;

private:
    Renderer *_pRenderer;
};

class MyAppDelegate : public NS::ApplicationDelegate {
public:
    ~MyAppDelegate() override;

    NS::Menu *createMenuBar();

    void applicationWillFinishLaunching(NS::Notification *pNotification) override;
    void applicationDidFinishLaunching(NS::Notification *pNotification) override;
    bool applicationShouldTerminateAfterLastWindowClosed(NS::Application *pSender) override;

private:
    NS::Window *_pWindow;
    MTK::View *_pMtkView;
    MTL::Device *_pDevice;
    MyMTKViewDelegate *_pViewDelegate = nullptr;
};

}// namespace vox