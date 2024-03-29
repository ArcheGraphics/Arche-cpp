//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/timeCode.h>
#include <pxr/usd/usdGeom/gprim.h>
#include <pxr/usd/sdf/propertySpec.h>
#include <pxr/base/vt/value.h>
#include "commands_impl.h"
#include "command_stack.h"
#include "sdf_command_group_recorder.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
/// Due to static_assert in the attribute.h file, the compiler is not able find the correct UsdAttribute::Set function
/// so we had to create a specific command for this

struct AttributeSet : public SdfLayerCommand {
    AttributeSet(const UsdAttribute &attribute, VtValue value, UsdTimeCode currentTime)
        : _stage(attribute.GetStage()), _path(attribute.GetPath()), _value(std::move(value)), _timeCode(currentTime) {}

    ~AttributeSet() override = default;

    bool do_it() override {
        if (_stage) {
            auto layer = _stage->GetEditTarget().GetLayer();
            if (layer) {
                SdfCommandGroupRecorder recorder(_undoCommands, layer);
                const UsdAttribute &attribute = _stage->GetAttributeAtPath(_path);
                attribute.Set(_value, _timeCode);
                return true;
            }
        }
        return false;
    }

    UsdStageWeakPtr _stage;
    SdfPath _path;
    VtValue _value;
    UsdTimeCode _timeCode;
};
template void execute_after_draw<AttributeSet>(UsdAttribute attribute, VtValue value, UsdTimeCode currentTime);

struct AttributeCreateDefaultValue : public SdfLayerCommand {
    explicit AttributeCreateDefaultValue(const UsdAttribute &attribute)
        : _stage(attribute.GetStage()), _path(attribute.GetPath()) {}

    ~AttributeCreateDefaultValue() override = default;

    bool do_it() override {
        if (_stage) {
            auto layer = _stage->GetEditTarget().GetLayer();
            if (layer) {
                SdfCommandGroupRecorder recorder(_undoCommands, layer);
                const UsdAttribute &attribute = _stage->GetAttributeAtPath(_path);
                if (attribute) {
                    VtValue value = attribute.GetTypeName().GetDefaultValue();
                    if (value.IsHolding<VtArray<GfVec3f>>() && attribute.GetRoleName() == TfToken("Color"))
                        value = VtArray<GfVec3f>{{0.0, 0.0, 0.0}};
                    if (value != VtValue()) {
                        // This will override the default value if there is one already
                        attribute.Set(value, UsdTimeCode::Default());
                        return true;
                    }
                }
            }
        }
        return false;
    }

    UsdStageWeakPtr _stage;
    SdfPath _path;
};
template void execute_after_draw<AttributeCreateDefaultValue>(UsdAttribute attribute);

}// namespace vox