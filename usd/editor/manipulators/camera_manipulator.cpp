//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pxr/usd/usdGeom/camera.h>
#include "camera_manipulator.h"
#include "viewport.h"
#include "commands/commands.h"
#include <imgui.h>

namespace vox {
CameraManipulator::CameraManipulator(const GfVec2i &viewportSize, bool isZUp) : CameraRig(viewportSize, isZUp) {}

void CameraManipulator::on_begin_edition(Viewport &viewport) {
    _stageCamera = viewport.get_usd_geom_camera();
    if (_stageCamera) {
        begin_edition(viewport.get_current_stage());
    }
}

void CameraManipulator::on_end_edition(Viewport &viewport) {
    if (_stageCamera) {
        end_edition();
    }
}

Manipulator *CameraManipulator::on_update(Viewport &viewport) {
    auto &cameraManipulator = viewport.get_camera_manipulator();
    ImGuiIO &io = ImGui::GetIO();

    /// If the user released key alt, escape camera manipulation
    if (!ImGui::IsKeyDown(ImGuiKey_LeftAlt)) {
        return viewport.get_manipulator<MouseHoverManipulator>();
    } else if (ImGui::IsMouseReleased(1) || ImGui::IsMouseReleased(2) || ImGui::IsMouseReleased(0)) {
        set_movement_type(MovementType::None);
    } else if (ImGui::IsMouseClicked(0)) {
        set_movement_type(MovementType::Orbit);
    } else if (ImGui::IsMouseClicked(2)) {
        set_movement_type(MovementType::Truck);
    } else if (ImGui::IsMouseClicked(1)) {
        set_movement_type(MovementType::Dolly);
    }
    auto &currentCamera = viewport.get_current_camera();

    if (move(currentCamera, io.MouseDelta.x, io.MouseDelta.y)) {
        if (_stageCamera) {
            // This is going to fill the undo/redo buffer :S
            _stageCamera.SetFromCamera(currentCamera, viewport.get_current_time_code());
        }
    }

    return this;
}

}// namespace vox