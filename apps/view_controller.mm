//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#import "view_controller.h"
#import "camera.h"
#import "renderer.h"
#import "view_delegate.h"
#import <CoreImage/CoreImage.h>
#import <CoreFoundation/CoreFoundation.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>

using namespace vox;

@implementation AAPLViewController {
    MTKView *_view;
    ViewDelegate* _renderer;
    CGPoint _mouseCoord;
}

/// Initialize the view, and load the renderer.
- (void)viewDidLoad {
    [super viewDidLoad];

    _view = (MTKView *)self.view;
    _view.layer.backgroundColor = NSColor.clearColor.CGColor;

    _renderer = [[ViewDelegate alloc] initWithMetalKitView:_view];

    _view.delegate = _renderer;

    [_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];
}

/// On a mouse button down, record the starting point for a drag.
- (void)mouseDown:(NSEvent *)event {
    _mouseCoord = [self.view convertPoint:event.locationInWindow fromView:nil];
}

/// On a mouse button down event, records the starting point for a drag.
- (void)mouseDragged:(NSEvent *)event {
    CGPoint newCoord = [self.view convertPoint:event.locationInWindow fromView:nil];

    double dX = newCoord.x - _mouseCoord.x;
    double dY = newCoord.y - _mouseCoord.y;

    if (event.modifierFlags & NSEventModifierFlagOption) {
        double magnification = event.modifierFlags & NSEventModifierFlagShift ? 2.0 : 0.5;
//        _renderer->get_app()->viewCamera()->panByDelta({-dX * magnification, -dY * magnification});
    } else {
//        _renderer->get_app()->viewCamera()->panByDelta({-dX * 0.5, dY * 0.5});
    }

    _mouseCoord = newCoord;
}

/// Handles the magnification gesture.
- (void)magnifyWithEvent:(NSEvent *)event {
    double delta = -event.magnification;
    double magnification = event.modifierFlags & NSEventModifierFlagShift ? 160 : 16;

//    _renderer->get_app()->viewCamera()->zoomByDelta(delta * magnification);
}

/// Adjusts the zoom of the camera if the user holds down a modifier key; otherwise, pans the camera.
- (void)scrollWheel:(NSEvent *)event {
    if (event.modifierFlags & NSEventModifierFlagOption) {
        double delta = 5.0 * event.deltaY;

//        _renderer->get_app()->viewCamera()->zoomByDelta(delta);
    } else {
        double dX = event.deltaX;
        double dY = event.deltaY;

        double magnification = event.modifierFlags & NSEventModifierFlagShift ? 5.0 : 1.0;

//        _renderer->get_app()->viewCamera()->panByDelta({-dX * magnification, dY * magnification});
    }
}

@end
