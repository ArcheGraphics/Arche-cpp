//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "delegate.h"
#include "renderer.h"

#include "framework/common/logging.h"
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace vox {

MyMTKViewDelegate::MyMTKViewDelegate(MTK::View *view)
    : MTK::ViewDelegate(), _pRenderer(std::make_unique<Renderer>(view)) {
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

    _pRenderer->setupScene("/Users/yangfeng/Downloads/Kitchen_set/Kitchen_set.usd");
}

MyMTKViewDelegate::~MyMTKViewDelegate() {
    _pRenderer.reset();
    spdlog::drop_all();
}

void MyMTKViewDelegate::drawInMTKView(MTK::View *pView) {
    _pRenderer->draw(pView);
}

void MyMTKViewDelegate::drawableSizeWillChange(MTK::View *pView, CGSize size) {
    _pRenderer->drawableSizeWillChange(pView, size);
}

}// namespace vox