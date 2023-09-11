//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
class Viewport;

/// Base class for a manipulator.
/// The manipulator can be seen as an editing state part of a FSM
/// It also knows how to draw itself in the viewport

/// This base class forces the override of OnUpdate which is called every frame.
/// The OnUpdate should return the new manipulator to use after the update or itself
/// if the state (Translate/Rotate) should be the same
struct Manipulator {
    virtual ~Manipulator() = default;

    /// Enter State
    virtual void on_begin_edition(Viewport &){};

    /// Exit State
    virtual void on_end_edition(Viewport &){};

    /// Next State
    virtual Manipulator *on_update(Viewport &) = 0;

    virtual bool is_mouse_over(const Viewport &) { return false; };

    /// Draw the translate manipulator as seen in the viewport
    virtual void on_draw_frame(const Viewport &){};

    /// Called when the viewport changes its selection
    virtual void on_selection_change(Viewport &){};

    // Manipulator axis, for convenience as
    typedef enum {
        XAxis = 0,
        YAxis,
        ZAxis,
        None,
    } ManipulatorAxis;
};
}// namespace vox
