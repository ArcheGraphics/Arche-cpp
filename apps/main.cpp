//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/gui/window.h"
#include "viewport.h"
#include "camera.h"

int main(int argc, char *argv[]) {
    NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();

    static constexpr uint32_t resolution = 1024u;
    vox::Window window{"USD Viewer", resolution, resolution};

    auto stage = pxr::UsdStage::Open("assets/Kitchen_set/Kitchen_set.usd");

    vox::Viewport renderer{window.native_handle(), resolution, resolution};
    renderer.setupScene(stage);

    window.set_scroll_callback([&](float dx, float dy) {
        renderer.viewCamera()->panByDelta({dx, dy});
    });

    while (!window.should_close()) {
        renderer.draw();
        window.poll_events();
    }

    pPool->release();
}