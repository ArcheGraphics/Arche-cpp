//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <pxr/usd/sdf/layerStateDelegate.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
TF_DECLARE_WEAK_AND_REF_PTRS(UndoRedoLayerStateDelegate);

class SdfCommandGroup;

class UndoRedoLayerStateDelegate : public SdfLayerStateDelegateBase {
public:
    static UndoRedoLayerStateDelegateRefPtr create(SdfCommandGroup &undoCommands);

    void set_clean();
    void set_dirty();

protected:
    explicit UndoRedoLayerStateDelegate(SdfCommandGroup &undoCommands)
        : _dirty(false), _undoCommands(undoCommands) {
    }

    // SdfLayerStateDelegateBase overrides
    bool _IsDirty() override;
    void _MarkCurrentStateAsClean() override;
    void _MarkCurrentStateAsDirty() override;

    void _OnSetLayer(const SdfLayerHandle &layer) override;

    void _OnSetField(
        const SdfPath &path,
        const TfToken &fieldName,
        const VtValue &value) override;

    void _OnSetField(
        const SdfPath &path,
        const TfToken &fieldName,
        const SdfAbstractDataConstValue &value) override;

    void _OnSetFieldDictValueByKey(
        const SdfPath &path,
        const TfToken &fieldName,
        const TfToken &keyPath,
        const VtValue &value) override;

    void _OnSetFieldDictValueByKey(
        const SdfPath &path,
        const TfToken &fieldName,
        const TfToken &keyPath,
        const SdfAbstractDataConstValue &value) override;

    void _OnSetTimeSample(
        const SdfPath &path,
        double time,
        const VtValue &value) override;

    void _OnSetTimeSample(
        const SdfPath &path,
        double time,
        const SdfAbstractDataConstValue &value) override;

    void _OnCreateSpec(
        const SdfPath &path,
        SdfSpecType specType,
        bool inert) override;

    void _OnDeleteSpec(
        const SdfPath &path,
        bool inert) override;

    void _OnMoveSpec(
        const SdfPath &oldPath,
        const SdfPath &newPath) override;

    void _OnPushChild(
        const SdfPath &path,
        const TfToken &fieldName,
        const TfToken &value) override;

    void _OnPushChild(
        const SdfPath &path,
        const TfToken &fieldName,
        const SdfPath &value) override;

    void _OnPopChild(
        const SdfPath &path,
        const TfToken &fieldName,
        const TfToken &oldValue) override;

    void _OnPopChild(
        const SdfPath &path,
        const TfToken &fieldName,
        const SdfPath &oldValue) override;

private:
    bool _dirty;
    SdfCommandGroup &_undoCommands;
    SdfLayerHandle _layer;
};

}// namespace vox