//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <iostream>
#include <utility>
#include <pxr/usd/sdf/abstractData.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/layerStateDelegate.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
struct UndoRedoSetField {
    UndoRedoSetField(const SdfLayerHandle &layer, SdfPath path, TfToken fieldName, VtValue newValue, VtValue previousValue)
        : _layer(layer), _path(std::move(path)), _fieldName(std::move(fieldName)), _newValue(std::move(newValue)), _previousValue(std::move(previousValue)) {}

    UndoRedoSetField(UndoRedoSetField &&) noexcept = default;
    ~UndoRedoSetField() = default;

    void do_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->SetField(_path, _fieldName, _newValue);
        }
    }

    void undo_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->SetField(_path, _fieldName, _previousValue);
        }
    }

    SdfLayerRefPtr _layer;
    const SdfPath _path;
    const TfToken _fieldName;
    VtValue _newValue;
    VtValue _previousValue;
};

struct UndoRedoSetFieldDictValueByKey {
    UndoRedoSetFieldDictValueByKey(const SdfLayerHandle &layer, SdfPath path,
                                   TfToken fieldName, TfToken keyPath,
                                   VtValue value, VtValue previousValue)
        : _layer(layer), _path(std::move(path)), _fieldName(std::move(fieldName)),
          _keyPath(std::move(keyPath)), _newValue(std::move(value)),
          _previousValue(std::move(previousValue)) {}

    UndoRedoSetFieldDictValueByKey(UndoRedoSetFieldDictValueByKey &&) noexcept = default;
    ~UndoRedoSetFieldDictValueByKey() = default;

    void do_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->SetFieldDictValueByKey(_path, _fieldName, _keyPath, _newValue);
        }
    }

    void undo_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->SetFieldDictValueByKey(_path, _fieldName, _keyPath, _previousValue);
        }
    }

    SdfLayerRefPtr _layer;
    const SdfPath _path;
    const TfToken _fieldName;
    const TfToken _keyPath;
    VtValue _newValue;
    VtValue _previousValue;
};

struct UndoRedoSetTimeSample {
    UndoRedoSetTimeSample(const SdfLayerHandle &layer, const SdfPath &path, double timeCode, VtValue newValue)
        : _layer(layer), _path(path), _timeCode(timeCode), _newValue(std::move(newValue)), _isKeyFrame(false),
          _hasTimeSamples(false) {

        if (_layer && _layer->HasField(path, SdfFieldKeys->TimeSamples)) {
            _hasTimeSamples = true;
            _isKeyFrame = _layer->QueryTimeSample(_path, _timeCode, &_previousValue);
        }
    }
    ~UndoRedoSetTimeSample() = default;
    UndoRedoSetTimeSample(UndoRedoSetTimeSample &&) noexcept = default;

    void do_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->SetTimeSample(_path, _timeCode, _newValue);
        }
    }

    void undo_it() {
        if (_layer && _layer->GetStateDelegate()) {
            if (_hasTimeSamples && _isKeyFrame) {
                _layer->GetStateDelegate()->SetTimeSample(_path, _timeCode, _previousValue);
            } else if (_hasTimeSamples && !_isKeyFrame) {
                _layer->EraseTimeSample(_path, _timeCode);
            } else if (!_hasTimeSamples) {
                _layer->GetStateDelegate()->SetField(_path, SdfFieldKeys->TimeSamples, _previousValue);
            } else {
                // This shouldn't happen
            }
        }
    }

    // TODO: look for reducing the size of this struct
    SdfLayerRefPtr _layer;
    const SdfPath _path;
    double _timeCode;
    VtValue _newValue;
    VtValue _previousValue;
    bool _isKeyFrame;
    bool _hasTimeSamples;
};

struct UndoRedoCreateSpec {
    UndoRedoCreateSpec(const SdfLayerHandle &layer, SdfPath path, SdfSpecType specType, bool inert)
        : _layer(layer), _path(std::move(path)), _specType(specType), _inert(inert) {}

    void do_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->CreateSpec(_path, _specType, _inert);
        }
    }

    void undo_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->DeleteSpec(_path, _inert);
        }
    }

    SdfLayerRefPtr _layer;
    const SdfPath _path;
    const SdfSpecType _specType;
    const bool _inert;
};

struct UndoRedoDeleteSpec {
    // Need a structure to copy the old data
    struct _SpecCopier : public SdfAbstractDataSpecVisitor {
        explicit _SpecCopier(SdfAbstractData *dst_);
        bool VisitSpec(const SdfAbstractData &src, const SdfPath &path) override;
        void Done(const SdfAbstractData &) override;

        SdfAbstractData *const dst;
    };

    UndoRedoDeleteSpec(const SdfLayerHandle &layer, const SdfPath &path, bool inert, SdfAbstractDataPtr layerData);

    void do_it();
    void undo_it();

    SdfLayerRefPtr _layer;
    const SdfPath _path;
    const bool _inert;

    SdfAbstractDataPtr _layerData;// TODO: this might change ? isn't it ? normally it's retrieved from the delegate
    const SdfSpecType _deletedSpecType;
    SdfDataRefPtr _deletedData;
};

struct UndoRedoMoveSpec {
    UndoRedoMoveSpec(const SdfLayerHandle &layer, SdfPath oldPath, SdfPath newPath)
        : _layer(layer), _oldPath(std::move(oldPath)), _newPath(std::move(newPath)) {}

    void do_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->MoveSpec(_oldPath, _newPath);
        }
    };
    void undo_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->MoveSpec(_newPath, _oldPath);
        }
    };

    SdfLayerRefPtr _layer;
    const SdfPath _oldPath;
    const SdfPath _newPath;
};

template<typename ValueT>
struct UndoRedoPushChild {
    UndoRedoPushChild(const SdfLayerHandle &layer, SdfPath parentPath, TfToken fieldName, const ValueT &value)
        : _layer(layer), _parentPath(std::move(parentPath)), _fieldName(std::move(fieldName)), _value(value) {}

    void undo_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->PopChild(_parentPath, _fieldName, _value);
        }
    }

    void do_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->PushChild(_parentPath, _fieldName, _value);
        }
    }

    SdfLayerRefPtr _layer;
    const SdfPath _parentPath;
    const TfToken _fieldName;
    const ValueT _value;
};

template<typename ValueT>
struct UndoRedoPopChild {
    UndoRedoPopChild(const SdfLayerHandle &layer, SdfPath parentPath, TfToken fieldName, const ValueT &value)
        : _layer(layer), _parentPath(std::move(parentPath)), _fieldName(std::move(fieldName)), _value(value) {}

    void undo_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->PushChild(_parentPath, _fieldName, _value);
        }
    }

    void do_it() {
        if (_layer && _layer->GetStateDelegate()) {
            _layer->GetStateDelegate()->PopChild(_parentPath, _fieldName, _value);
        }
    }

    SdfLayerRefPtr _layer;
    const SdfPath _parentPath;
    const TfToken _fieldName;
    const ValueT _value;
};

}// namespace vox