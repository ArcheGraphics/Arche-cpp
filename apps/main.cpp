//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cassert>

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include "framework/common/logging.h"
#include "framework/platform/platform.h"
#include "framework/platform/unix_platform.h"

#include "primitive_app.h"

int main(int argc, char *argv[]) {
    NS::AutoreleasePool *pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

    vox::UnixPlatform platform;

    auto code = platform.initialize();

    if (code == vox::ExitCode::Success) {
        auto app = std::make_unique<vox::PrimitiveApp>();
        app->prepare(platform);
        platform.set_callback([&](float delta_time) { app->update(delta_time); },
                              [&](uint32_t width, uint32_t height) {
                                  app->resize(width, height, 1, 1);
                              },
                              [&](const vox::InputEvent &event) {
                                  app->input_event(event);
                              });

        // loop
        code = platform.main_loop();
    }

    platform.terminate(code);

    pAutoreleasePool->release();

    return 0;
}