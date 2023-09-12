//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/layer.h>
#include "../selection.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
void draw_sdf_attribute_editor(const SdfLayerHandle& layer, const Selection &);

void draw_time_sample_creation_dialog(const SdfLayerHandle& layer, const SdfPath& attributePath);

}// namespace vox