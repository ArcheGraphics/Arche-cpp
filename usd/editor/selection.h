//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
///
/// Selection api used by the widgets and the editor.
/// The code in Selection.cpp can be replaced if the widgets are to be used in a different application
/// managing its own selection mechanism
///
using SelectionHash = std::size_t;

struct Selection {
    Selection();
    ~Selection();

    // The selections are store by Owners which are Layers or Stages, meaning each layer should have its own selection
    // at some point. The API allows it, the code doesn't yet.
    // An Item is a combination of a Owner + SdfPath. If the stage is the Owner, then the Item is a UsdPrim

    template<typename OwnerT>
    void clear(const OwnerT &);
    template<typename OwnerT>
    void add_selected(const OwnerT &, const SdfPath &path);
    template<typename OwnerT>
    void remove_selected(const OwnerT &, const SdfPath &path);
    template<typename OwnerT>
    void set_selected(const OwnerT &, const SdfPath &path);
    template<typename OwnerT>
    bool is_selection_empty(const OwnerT &) const;
    template<typename OwnerT>
    bool is_selected(const OwnerT &, const SdfPath &path) const;
    template<typename ItemT>
    bool is_selected(const ItemT &) const;
    template<typename OwnerT>
    bool update_selection_hash(const OwnerT &, SelectionHash &lastSelectionHash);
    template<typename OwnerT>
    SdfPath get_anchor_prim_path(const OwnerT &) const;
    template<typename OwnerT>
    SdfPath get_anchor_property_path(const OwnerT &) const;
    template<typename OwnerT>
    std::vector<SdfPath> get_selected_paths(const OwnerT &) const;

    // private:
    struct SelectionData;
    SelectionData *_data;
};

}// namespace vox