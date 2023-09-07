//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "platform.h"

#include <algorithm>
#include <ctime>
#include <mutex>
#include <vector>
#include <iostream>

namespace vox {
const uint32_t Platform::MIN_WINDOW_WIDTH = 420;
const uint32_t Platform::MIN_WINDOW_HEIGHT = 320;
std::vector<std::string> Platform::_arguments = {};
std::string Platform::_externalStorageDirectory = "";
std::string Platform::_tempDirectory = "";

ExitCode Platform::initialize() {
    google::InitGoogleLogging("DigitalVox");

    createWindow(_windowProperties);
    if (!_window) {
        LOG(ERROR) << "Window creation failed!";
        return ExitCode::FatalError;
    }

    return ExitCode::Success;
}

ExitCode Platform::mainLoop() {
    // Load the requested app
    if (!startApp()) {
        LOG(ERROR) << "Failed to load requested application";
        return ExitCode::FatalError;
    }

    // Compensate for load times of the app by rendering the first frame pre-emptively
    _timer.tick<Timer::Seconds>();
    _activeApp->update(0.01667f);

    while (!_window->shouldClose()) {
        try {
            update();

            _window->processEvents();
        } catch (std::exception &e) {
            LOG(ERROR) << "Error Message: " << e.what();
            LOG(ERROR) << "Failed when running application " << _activeApp->name();

            return ExitCode::FatalError;
        }
    }

    return ExitCode::Success;
}

void Platform::update() {
    auto delta_time = static_cast<float>(_timer.tick<Timer::Seconds>());

    if (_focused) {
        if (_fixedSimulationFPS) {
            delta_time = _simulationFrameTime;
        }

        _activeApp->update(delta_time);
    }
}

void Platform::terminate(ExitCode code) {
    if (_activeApp) {
        _activeApp->finish();
    }

    _activeApp.reset();
    _window.reset();

    // Halt on all unsuccessful exit codes unless ForceClose is in use
    if (code != ExitCode::Success) {
        std::cout << "Press any key to continue";
        std::cin.get();
    }
}

void Platform::close() {
    if (_window) {
        _window->close();
    }
}

void Platform::inputEvent(const InputEvent &inputEvent) {
    if (_processInputEvents && _activeApp) {
        _activeApp->inputEvent(inputEvent);
    }

    if (inputEvent.source() == EventSource::Keyboard) {
        const auto &key_event = static_cast<const KeyInputEvent &>(inputEvent);

        if (key_event.code() == KeyCode::Back ||
            key_event.code() == KeyCode::Escape) {
            close();
        }
    }
}

void Platform::resize(uint32_t win_width, uint32_t win_height,
                    uint32_t fb_width, uint32_t fb_height) {
    auto extent = Window::Extent{std::max<uint32_t>(win_width, MIN_WINDOW_WIDTH), std::max<uint32_t>(win_height, MIN_WINDOW_HEIGHT)};
    if (_window) {
        _window->resize(extent);
        if (_activeApp) {
            _activeApp->resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

std::unique_ptr<RenderContext> Platform::createRenderContext(MTL::Device &device) {
    return _window->createRenderContext(device);
}

//MARK: - Application
Window &Platform::window() const {
    return *_window;
}

Application &Platform::app() {
    assert(_activeApp && "Application is not valid");
    return *_activeApp;
}

Application &Platform::app() const {
    assert(_activeApp && "Application is not valid");
    return *_activeApp;
}

void Platform::setApp(std::unique_ptr<Application> &&new_app) {
    if (_activeApp) {
        auto execution_time = _timer.stop();
        LOG(INFO) << "Closing App (Runtime: " << execution_time << ")";

        auto app_id = _activeApp->name();

        _activeApp->finish();
    }

    _activeApp = std::move(new_app);
}

bool Platform::startApp() {
    _activeApp->setName("");

    if (!_activeApp) {
        LOG(ERROR) << "Failed to create a valid metal app.";
        return false;
    }

    if (!_activeApp->prepare(*this)) {
        LOG(ERROR) << "Failed to prepare metal app.";
        return false;
    }

    return true;
}

//MARK: - Controller
void Platform::forceSimulationFPS(float fps) {
    _fixedSimulationFPS = true;
    _simulationFrameTime = 1 / fps;
}

void Platform::disableInputProcessing() {
    _processInputEvents = false;
}

void Platform::setFocus(bool focused) {
    _focused = focused;
}

//MARK: - Property
void Platform::setWindowProperties(const Window::OptionalProperties &properties) {
    _windowProperties.title = properties.title.has_value() ? properties.title.value() : _windowProperties.title;
    _windowProperties.mode = properties.mode.has_value() ? properties.mode.value() : _windowProperties.mode;
    _windowProperties.resizable = properties.resizable.has_value() ? properties.resizable.value() : _windowProperties.resizable;
    _windowProperties.vsync = properties.vsync.has_value() ? properties.vsync.value() : _windowProperties.vsync;
    _windowProperties.extent.width = properties.extent.width.has_value() ? properties.extent.width.value() : _windowProperties.extent.width;
    _windowProperties.extent.height = properties.extent.height.has_value() ? properties.extent.height.value() : _windowProperties.extent.height;
}

const std::string &Platform::externalStorageDirectory() {
    return _externalStorageDirectory;
}

const std::string &Platform::tempDirectory() {
    return _tempDirectory;
}

void Platform::setArguments(const std::vector<std::string> &args) {
    _arguments = args;
}

void Platform::setExternalStorageDirectory(const std::string &dir) {
    _externalStorageDirectory = dir;
}

void Platform::setTempDirectory(const std::string &dir) {
    _tempDirectory = dir;
}

}// namespace vox
