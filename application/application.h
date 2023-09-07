//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "input_events.h"
#include "framework/common/timer.h"

namespace vox {
class Window;

class Engine;

class Application {
public:
    Application();

    virtual ~Application() = default;

    /**
     * @brief Prepares the application for execution
     * @param engine The engine the application is being run on
     */
    virtual bool prepare(Engine &engine);

    /**
     * @brief Updates the application
     * @param delta_time The time since the last update
     */
    virtual void update(float delta_time);

    /**
     * @brief Handles cleaning up the application
     */
    virtual void finish();

    /**
     * @brief Handles resizing of the window
     * @param win_width New width of the window
     * @param win_height New height of the window
     * @param fb_width New width of the framebuffer
     * @param fb_height New height of the framebuffer
     */
    virtual bool resize(uint32_t win_width, uint32_t win_height,
                        uint32_t fb_width, uint32_t fb_height);

    /**
     * @brief Handles input events of the window
     * @param inputEvent The input event object
     */
    virtual void inputEvent(const InputEvent &inputEvent);

    const std::string &name() const;

    void setName(const std::string &name);

protected:
    float _fps{0.0f};

    float _frameTime{0.0f};// In ms

    uint32_t _frameCount{0};

    uint32_t _lastFrameCount{0};

    Engine *_engine{nullptr};

private:
    std::string _name{};
};

}// namespace vox
