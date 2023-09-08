//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#import <MetalKit/MTKView.h>

@interface ViewDelegate : NSObject<MTKViewDelegate>

/// Initializes the view.
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView*)view;

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size;

- (void)drawInMTKView:(nonnull MTKView *)view;

@end
