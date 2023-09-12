//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <pxr/usd/sdf/primSpec.h>
#include <pxr/usd/sdf/reference.h>
#include "commands_impl.h"
#include "sdf_undo_redo_recorder.h"
#include <pxr/usd/sdf/variantSpec.h>

#include <utility>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
struct LayerRemoveSubLayer : public SdfLayerCommand {
    // Removes a sublayer
    LayerRemoveSubLayer(SdfLayerRefPtr layer, std::string subLayerPath) : _layer(std::move(layer)), _subLayerPath(std::move(subLayerPath)) {}

    ~LayerRemoveSubLayer() override = default;

    bool do_it() override {
        if (!_layer)
            return false;
        SdfCommandGroupRecorder recorder(_undoCommands, _layer);
        for (int i = 0; i < _layer->GetNumSubLayerPaths(); i++) {
            if (_layer->GetSubLayerPaths()[i] == _subLayerPath) {
                _layer->RemoveSubLayerPath(i);
                return true;
            }
        }
        return false;
    }
    SdfLayerRefPtr _layer;
    std::string _subLayerPath;
};
template void execute_after_draw<LayerRemoveSubLayer>(SdfLayerRefPtr layer, std::string subLayerPath);

/// Change layer position in the layer stack, moving up and down
struct LayerMoveSubLayer : public SdfLayerCommand {
    // Removes a sublayer
    LayerMoveSubLayer(SdfLayerRefPtr layer, std::string subLayerPath, bool movingUp)
        : _layer(std::move(std::move(layer))), _subLayerPath(std::move(subLayerPath)), _movingUp(movingUp) {}

    ~LayerMoveSubLayer() override = default;

    bool do_it() override {
        SdfCommandGroupRecorder recorder(_undoCommands, _layer);
        return _movingUp ? MoveUp() : MoveDown();
    }

    [[nodiscard]] bool MoveUp() const {
        if (!_layer)
            return false;
        std::vector<std::string> layers = _layer->GetSubLayerPaths();
        for (size_t i = 1; i < layers.size(); i++) {
            if (layers[i] == _subLayerPath) {
                std::swap(layers[i], layers[i - 1]);
                _layer->SetSubLayerPaths(layers);
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool MoveDown() const {
        if (!_layer)
            return false;
        std::vector<std::string> layers = _layer->GetSubLayerPaths();
        for (size_t i = 0; i < layers.size() - 1; i++) {
            if (layers[i] == _subLayerPath) {
                std::swap(layers[i], layers[i + 1]);
                _layer->SetSubLayerPaths(layers);
                return true;
            }
        }
        return false;
    }

    SdfLayerRefPtr _layer;
    std::string _subLayerPath;
    bool _movingUp;/// Template instead ?
};
template void execute_after_draw<LayerMoveSubLayer>(SdfLayerRefPtr layer, std::string subLayerPath, bool movingUp);

/// Rename a sublayer
struct LayerRenameSubLayer : public SdfLayerCommand {
    // Removes a sublayer
    LayerRenameSubLayer(SdfLayerRefPtr layer, std::string oldName, std::string newName)
        : _layer(std::move(std::move(std::move(std::move(layer))))), _oldName(std::move(oldName)), _newName(std::move(newName)) {}

    ~LayerRenameSubLayer() override = default;

    bool do_it() override {
        SdfCommandGroupRecorder recorder(_undoCommands, _layer);
        if (!_layer)
            return false;
        std::vector<std::string> layers = _layer->GetSubLayerPaths();
        for (size_t i = 0; i < layers.size(); i++) {
            if (layers[i] == _oldName) {
                layers[i] = _newName;
                _layer->SetSubLayerPaths(layers);
                return true;
            }
        }
        return false;
    }

    SdfLayerRefPtr _layer;
    std::string _oldName;
    std::string _newName;
};
template void execute_after_draw<LayerRenameSubLayer>(SdfLayerRefPtr layer, std::string oldName, std::string newName);

/// Mute and Unmute seem to keep so additional data outside of Sdf, so they need their own commands
struct LayerMute : public Command {
    explicit LayerMute(SdfLayerRefPtr layer) : _layer(std::move(layer)) {}
    explicit LayerMute(const SdfLayerHandle &layer) : _layer(layer) {}
    bool do_it() override {
        if (!_layer)
            return false;
        _layer->SetMuted(true);
        return true;
    };
    bool undo_it() override {
        if (_layer)
            _layer->SetMuted(false);
        return false;
    }
    SdfLayerRefPtr _layer;
};
template void execute_after_draw<LayerMute>(SdfLayerRefPtr layer);
template void execute_after_draw<LayerMute>(SdfLayerHandle layer);

struct LayerUnmute : public Command {
    explicit LayerUnmute(SdfLayerRefPtr layer) : _layer(std::move(layer)) {}
    explicit LayerUnmute(const SdfLayerHandle &layer) : _layer(layer) {}
    bool do_it() override {
        if (!_layer)
            return false;
        _layer->SetMuted(false);
        return true;
    };
    bool undo_it() override {
        if (_layer)
            _layer->SetMuted(true);
        return false;
    }
    SdfLayerRefPtr _layer;
};
template void execute_after_draw<LayerUnmute>(SdfLayerRefPtr layer);
template void execute_after_draw<LayerUnmute>(SdfLayerHandle layer);

/* WARNING: this is a brute force and dumb implementation of storing text modification.
 It basically stores the previous and new layer as text in a string. So .... this will eat up the memory
 quite quickly if used intensively.
 But for now it's a quick way to test if text editing is worth in the application.
 */
struct LayerTextEdit : public SdfLayerCommand {
    LayerTextEdit(SdfLayerRefPtr layer, std::string newText) : _layer(std::move(layer)), _newText(std::move(newText)) {}

    ~LayerTextEdit() override = default;

    bool do_it() override {
        if (!_layer)
            return false;
        SdfCommandGroupRecorder recorder(_undoCommands, _layer);
        if (_oldText.empty()) {
            _layer->ExportToString(&_oldText);
        }
        return _layer->ImportFromString(_newText);
        //_layer->SetDirty();
    };

    bool undo_it() override {
        if (!_layer)
            return false;
        return _layer->ImportFromString(_oldText);
    }

    SdfLayerRefPtr _layer;
    std::string _oldText;
    std::string _newText;
};
template void execute_after_draw<LayerTextEdit>(SdfLayerRefPtr layer, std::string newText);

struct LayerCreateOversFromPath : public SdfLayerCommand {
    LayerCreateOversFromPath(SdfLayerRefPtr layer, std::string path) : _layer(std::move(layer)), _path(std::move(path)) {}
    ~LayerCreateOversFromPath() override = default;

    bool do_it() override {
        if (!_layer)
            return false;

        SdfPath path(_path);
        if (path == SdfPath()) {
            return false;
        }
        SdfCommandGroupRecorder recorder(_undoCommands, _layer);

        for (auto &prefix : path.GetPrefixes()) {
            if (!_layer->HasSpec(prefix)) {
                if (prefix.GetParentPath().IsAbsoluteRootPath()) {
                    SdfPrimSpec::New(_layer, prefix.GetName(), SdfSpecifierOver);
                } else if (prefix.IsPrimVariantSelectionPath()) {
                    auto variant = prefix.GetVariantSelection();
                    SdfCreateVariantInLayer(_layer, prefix.GetParentPath(), variant.first, variant.second);
                } else {
                    SdfPrimSpec::New(_layer->GetPrimAtPath(prefix.GetParentPath()), prefix.GetName(), SdfSpecifierOver);
                }
            }
        }
        return true;
    }

    SdfLayerRefPtr _layer;
    std::string _path;
};
template void execute_after_draw<LayerCreateOversFromPath>(SdfLayerRefPtr layer, std::string path);

}// namespace vox