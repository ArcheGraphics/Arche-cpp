//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/base/gf/vec3f.h>

PXR_NAMESPACE_USING_DIRECTIVE

#include "manipulator.h"

namespace vox {
/// The selection manipulator will help selecting a region of the viewport, drawing a rectangle.
class SelectionManipulator : public Manipulator {
public:
    SelectionManipulator() = default;
    ~SelectionManipulator() = default;

    void on_draw_frame(const Viewport &) override;

    Manipulator *on_update(Viewport &) override;

    // Picking modes
    enum class PickMode { Prim,
                          Model,
                          Assembly };
    void set_pick_mode(PickMode pickMode) { _pickMode = pickMode; }
    PickMode get_pick_mode() const { return _pickMode; }

private:
    // Returns true
    bool is_pickable_path(const class UsdStage &stage, const class SdfPath &path) const;
    PickMode _pickMode = PickMode::Prim;
};

/// Draw an ImGui menu to select the picking mode
void draw_pick_mode(SelectionManipulator &manipulator);

}// namespace vox