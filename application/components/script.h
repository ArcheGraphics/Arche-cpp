//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ecs/component.h"
#include "platform/input_events.h"

namespace vox {
class Camera;

/**
 * Script class, used for logic writing.
 */
class Script : public Component {
public:
    explicit Script(Entity *entity);

    ~Script() override;

    /**
     * Called when be enabled first time, only once.
     */
    virtual void on_script_awake() {}

    /**
     * Called when be enabled.
     */
    virtual void on_script_enable() {}

    /**
     * Called when be disabled.
     */
    virtual void on_script_disable() {}

    /**
     * Called at the end of the destroyed frame.
     */
    virtual void on_destroy() {}

public:
    void set_is_started(bool value);

    [[nodiscard]] bool is_started() const;

    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    virtual void on_start() {}

    /**
     * The main loop, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    virtual void on_update(float delta_time) {}

    /**
     * Called after the OnUpdate finished, called frame by frame.
     * @param delta_time - The deltaTime when the script update.
     */
    virtual void on_late_update(float delta_time) {}

    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void on_begin_render(Camera *camera) {}

    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void on_end_render(Camera *camera) {}

    virtual void input_event(const InputEvent &input_event) {}

    virtual void resize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height) {}

protected:
    void on_awake() override;

    void on_enable() override;

    void on_disable() override;

    bool started_ = false;
};

}// namespace vox
