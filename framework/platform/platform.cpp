//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "platform.h"

#include <algorithm>
#include <utility>
#include <vector>

#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "common/logging.h"

namespace vox {
const uint32_t Platform::MIN_WINDOW_WIDTH = 420;
const uint32_t Platform::MIN_WINDOW_HEIGHT = 320;

std::string Platform::external_storage_directory;

std::string Platform::temp_directory;

ExitCode Platform::initialize() {
    auto sinks = get_platform_sinks();

    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());

#ifdef METAL_DEBUG
    logger->set_level(spdlog::level::debug);
#else
    logger->set_level(spdlog::level::info);
#endif

    logger->set_pattern(LOGGER_FORMAT);
    spdlog::set_default_logger(logger);

    LOGI("Logger initialized");

    // Platform has been closed by a plugins initialization phase
    if (close_requested) {
        return ExitCode::Close;
    }

    create_window(window_properties);

    if (!window) {
        LOGE("Window creation failed!");
        return ExitCode::FatalError;
    }

    return ExitCode::Success;
}

void Platform::set_callback(std::function<void(float)> update_callback,
                            std::function<void(uint32_t, uint32_t)> resize_callback,
                            std::function<void(const InputEvent &)> event_callback) {
    this->update_callback = std::move(update_callback);
    this->resize_callback = std::move(resize_callback);
    this->event_callback = std::move(event_callback);
}

ExitCode Platform::main_loop() {
    while (!window->should_close() && !close_requested) {
        try {
            update();
            window->process_events();
        } catch (std::exception &e) {
            LOGE("Error Message: {}", e.what())
        }
    }

    return ExitCode::Success;
}

void Platform::update() {
    auto delta_time = static_cast<float>(timer.tick<Timer::Seconds>());

    if (focused) {
        if (fixed_simulation_fps) {
            delta_time = simulation_frame_time;
        }

        update_callback(delta_time);
    }
}

void Platform::terminate(ExitCode code) {
    window.reset();
    spdlog::drop_all();
}

void Platform::close() {
    if (window) {
        window->close();
    }

    // Fallback incase a window is not yet in use
    close_requested = true;
}

void Platform::force_simulation_fps(float fps) {
    fixed_simulation_fps = true;
    simulation_frame_time = 1 / fps;
}

void Platform::disable_input_processing() {
    process_input_events = false;
}

void Platform::set_focus(bool _focused) {
    focused = _focused;
}

void Platform::set_window_properties(const Window::OptionalProperties &properties) {
    window_properties.title = properties.title.has_value() ? properties.title.value() : window_properties.title;
    window_properties.mode = properties.mode.has_value() ? properties.mode.value() : window_properties.mode;
    window_properties.resizable = properties.resizable.has_value() ? properties.resizable.value() : window_properties.resizable;
    window_properties.vsync = properties.vsync.has_value() ? properties.vsync.value() : window_properties.vsync;
    window_properties.extent.width = properties.extent.width.has_value() ? properties.extent.width.value() : window_properties.extent.width;
    window_properties.extent.height = properties.extent.height.has_value() ? properties.extent.height.value() : window_properties.extent.height;
}

const std::string &Platform::get_external_storage_directory() {
    return external_storage_directory;
}

const std::string &Platform::get_temp_directory() {
    return temp_directory;
}

Window &Platform::get_window() {
    return *window;
}

void Platform::set_external_storage_directory(const std::string &dir) {
    external_storage_directory = dir;
}

std::vector<spdlog::sink_ptr> Platform::get_platform_sinks() {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    return sinks;
}

void Platform::input_event(const InputEvent &input_event) {
    if (process_input_events) {
        event_callback(input_event);
    }

    if (input_event.get_source() == EventSource::Keyboard) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);

        if (key_event.get_code() == KeyCode::Back ||
            key_event.get_code() == KeyCode::Escape) {
            close();
        }
    }
}

void Platform::resize(uint32_t width, uint32_t height) {
    auto extent = Window::Extent{std::max<uint32_t>(width, MIN_WINDOW_WIDTH), std::max<uint32_t>(height, MIN_WINDOW_HEIGHT)};
    if ((window) && (width > 0) && (height > 0)) {
        auto actual_extent = window->resize(extent);
        resize_callback(actual_extent.width, actual_extent.height);
    }
}

}// namespace vox
