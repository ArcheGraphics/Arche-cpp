//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include <pxr/usd/sdf/listEditorProxy.h>
#include <pxr/usd/sdf/reference.h>
#include <pxr/usd/sdf/listOp.h>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
// ExtraArgsT is used to pass additional arguments as the function passed as visitor
// might need more than the operation and the item
template<typename PolicyT, typename FuncT, typename... ExtraArgsT>
static void iterate_list_editor_items(const SdfListEditorProxy<PolicyT> &listEditor, const FuncT &call, ExtraArgsT... args) {
    for (const typename PolicyT::value_type &item : listEditor.GetExplicitItems()) {
        call(SdfListOpTypeExplicit, item, args...);
    }
    for (const typename PolicyT::value_type &item : listEditor.GetOrderedItems()) {
        call(SdfListOpTypeOrdered, item, args...);
    }
    for (const typename PolicyT::value_type &item : listEditor.GetAddedItems()) {
        call(SdfListOpTypeAdded, item, args...);// return "add" as TfToken instead ?
    }
    for (const typename PolicyT::value_type &item : listEditor.GetPrependedItems()) {
        call(SdfListOpTypePrepended, item, args...);
    }
    for (const typename PolicyT::value_type &item : listEditor.GetAppendedItems()) {
        call(SdfListOpTypeAppended, item, args...);
    }
    for (const typename PolicyT::value_type &item : listEditor.GetDeletedItems()) {
        call(SdfListOpTypeDeleted, item, args...);
    }
}

/// The list available on a SdfListEditor
constexpr int get_list_editor_operation_size() { return 6; }

template<typename IntOrSdfListOpT>
inline const char *get_list_editor_operation_name(IntOrSdfListOpT index) {
    constexpr const char *names[get_list_editor_operation_size()] = {"explicit", "add", "delete", "ordered", "prepend", "append"};
    return names[static_cast<int>(index)];
}

template<typename IntOrSdfListOpT>
inline const char *get_list_editor_operation_abbreviation(IntOrSdfListOpT index) {
    constexpr const char *names[get_list_editor_operation_size()] = {"Ex", "Ad", "De", "Or", "Pr", "Ap"};
    return names[static_cast<int>(index)];
}

template<typename PolicyT>
inline void create_list_editor_operation(SdfListEditorProxy<PolicyT> &&listEditor, SdfListOpType op,
                                         typename SdfListEditorProxy<PolicyT>::value_type item) {
    switch (op) {
        case SdfListOpTypeAdded:
            listEditor.GetAddedItems().push_back(item);
            break;
        case SdfListOpTypePrepended:
            listEditor.GetPrependedItems().push_back(item);
            break;
        case SdfListOpTypeAppended:
            listEditor.GetAppendedItems().push_back(item);
            break;
        case SdfListOpTypeDeleted:
            listEditor.GetDeletedItems().push_back(item);
            break;
        case SdfListOpTypeExplicit:
            listEditor.GetExplicitItems().push_back(item);
            break;
        case SdfListOpTypeOrdered:
            listEditor.GetOrderedItems().push_back(item);
            break;
        default:
            assert(0);
    }
}

template<typename ListEditorT, typename OpOrIntT>
inline auto get_sdf_list_op_items(ListEditorT &listEditor, OpOrIntT op_) {
    const auto op = static_cast<SdfListOpType>(op_);
    if (op == SdfListOpTypeOrdered) {
        return listEditor.GetOrderedItems();
    } else if (op == SdfListOpTypeAppended) {
        return listEditor.GetAppendedItems();
    } else if (op == SdfListOpTypeAdded) {
        return listEditor.GetAddedItems();
    } else if (op == SdfListOpTypePrepended) {
        return listEditor.GetPrependedItems();
    } else if (op == SdfListOpTypeDeleted) {
        return listEditor.GetDeletedItems();
    }
    return listEditor.GetExplicitItems();
}

template<typename ListEditorT, typename OpOrIntT, typename ItemsT>
inline void set_sdf_list_op_items(ListEditorT &listEditor, OpOrIntT op_, const ItemsT &items) {
    const auto op = static_cast<SdfListOpType>(op_);
    if (op == SdfListOpTypeOrdered) {
        listEditor.SetOrderedItems(items);
    } else if (op == SdfListOpTypeAppended) {
        listEditor.SetAppendedItems(items);
    } else if (op == SdfListOpTypeAdded) {
        listEditor.SetAddedItems(items);
    } else if (op == SdfListOpTypePrepended) {
        listEditor.SetPrependedItems(items);
    } else if (op == SdfListOpTypeDeleted) {
        listEditor.SetDeletedItems(items);
    } else {
        listEditor.SetExplicitItems(items);
    }
}

// Look for a new token. If prefix ends with a number, it will increase its value until
// a valid token is found
std::string find_next_available_token_string(std::string prefix);

// Find usd file format extensions and returns them prefixed with a dot
std::vector<std::string> get_usd_valid_extensions();

}// namespace vox