//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <iostream>
#include <utility>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/abstractData.h>
#include "sdf_layer_instructions.h"

namespace vox {
namespace {
void _copy_spec(const SdfAbstractData &src, SdfAbstractData *dst, const SdfPath &path) {
    if (!dst) {
        std::cerr << "ERROR: when copying the destination prim is null at path " << path.GetString() << std::endl;
        return;
    }
    dst->CreateSpec(path, src.GetSpecType(path));
    const TfTokenVector &fields = src.List(path);
    TF_FOR_ALL(i, fields) { dst->Set(path, *i, src.Get(path, *i)); }
}
}// namespace

UndoRedoDeleteSpec::_SpecCopier::_SpecCopier(SdfAbstractData *dst_) : dst(dst_) {}

bool UndoRedoDeleteSpec::_SpecCopier::VisitSpec(const SdfAbstractData &src, const SdfPath &path) {
    _copy_spec(src, dst, path);
    return true;
}

void UndoRedoDeleteSpec::_SpecCopier::Done(const SdfAbstractData &) {}

UndoRedoDeleteSpec::UndoRedoDeleteSpec(const SdfLayerHandle &layer, const SdfPath &path, bool inert, SdfAbstractDataPtr layerData)
    : _layer(layer), _path(path), _inert(inert), _deletedSpecType(_layer->GetSpecType(path)), _layerData(std::move(layerData)) {
    // TODO: is there a faster way of copying and restoring the data ?
    // This can be really slow on big scenes
    SdfChangeBlock changeBlock;
    _deletedData = TfCreateRefPtr(new SdfData());
    SdfLayer::TraversalFunction copyFunc = std::bind(&_copy_spec, std::cref(*get_pointer(_layerData)),
                                                     get_pointer(_deletedData), std::placeholders::_1);
    _layer->Traverse(path, copyFunc);
}

void UndoRedoDeleteSpec::do_it() {
    if (_layer && _layer->GetStateDelegate()) {
        _layer->GetStateDelegate()->DeleteSpec(_path, _inert);
    }
}

void UndoRedoDeleteSpec::undo_it() {
    if (_layer && _layer->GetStateDelegate()) {
        SdfChangeBlock changeBlock;
        _SpecCopier copier(get_pointer(_layerData));
        _layer->GetStateDelegate()->CreateSpec(_path, _deletedSpecType, _inert);
        _deletedData->VisitSpecs(&copier);
    }
}

}// namespace vox