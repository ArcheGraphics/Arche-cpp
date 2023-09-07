//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "framework/platform/input_events.h"
#include "framework/common/timer.h"

namespace vox {
class Window;

class Application {
public:
    Application();

    virtual ~Application() = default;

    /**
	 * @brief Prepares the application for execution
	 */
    virtual bool prepare(Platform &engine);

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
	 * @param width New width of the window
	 * @param height New height of the window
	 */
    virtual bool resize(uint32_t width, uint32_t height, uint32_t fb_width, uint32_t fb_height);

    /**
	 * @brief Handles input events of the window
	 * @param input_event The input event object
	 */
    virtual void input_event(const InputEvent &input_event);

    [[nodiscard]] const std::string &get_name() const;

    void set_name(const std::string &name);

    [[nodiscard]] inline bool should_close() const {
        return requested_close;
    }

    // request the app to close
    // does not guarantee that the app will close immediately
    inline void close() {
        requested_close = true;
    }

protected:
    float fps{0.0f};

    float frame_time{0.0f};// In ms

    uint32_t frame_count{0};

    uint32_t last_frame_count{0};

    Window *window{nullptr};

private:
    std::string name{};

    bool requested_close{false};
};
}// namespace vox
