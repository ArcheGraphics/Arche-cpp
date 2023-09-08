//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "view_delegate.h"
#include "renderer.h"
#include "framework/common/logging.h"
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

using namespace vox;

@implementation ViewDelegate {
    std::unique_ptr<Renderer> _pRenderer;
}

/// Initializes the view.
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)view {
    if (self = [super init]) {
        _pRenderer = std::make_unique<Renderer>((MTK::View *)view);

        // setup logger
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());

#ifdef METAL_DEBUG
        logger->set_level(spdlog::level::debug);
#else
        logger->set_level(spdlog::level::info);
#endif

        logger->set_pattern(LOGGER_FORMAT);
        spdlog::set_default_logger(logger);

        _pRenderer->setupScene("assets/Kitchen_set/Kitchen_set.usd");
    }

    return self;
}

- (void)dealloc {
    _pRenderer.reset();
    spdlog::drop_all();
    [super dealloc];
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    _pRenderer->drawableSizeWillChange((MTK::View *)view, size);
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    _pRenderer->draw((MTK::View *)view);
}

@end