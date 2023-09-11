//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "camera_rig.h"
#include "manipulator.h"
#include <pxr/usd/usdGeom/camera.h>

namespace vox {
class CameraManipulator : public CameraRig, public Manipulator {
public:
    CameraManipulator(const GfVec2i &viewportSize, bool isZUp = false);

    void OnBeginEdition(Viewport &) override;
    Manipulator *OnUpdate(Viewport &) override;
    void OnEndEdition(Viewport &) override;

private:
    UsdGeomCamera _stageCamera;
};

}// namespace vox