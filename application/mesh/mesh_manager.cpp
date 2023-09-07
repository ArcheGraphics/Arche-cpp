//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "mesh/mesh_manager.h"

namespace vox {
MeshManager::MeshManager(MTL::Device &device) : device_(device) {}

MeshManager *MeshManager::get_singleton_ptr() { return ms_singleton; }

MeshManager &MeshManager::get_singleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

std::shared_ptr<ModelMesh> MeshManager::load_model_mesh() {
    auto mesh = std::make_shared<ModelMesh>(device_);
    mesh_pool_.emplace_back(mesh);
    return mesh;
}

void MeshManager::collect_garbage() {
    mesh_pool_.erase(std::remove_if(mesh_pool_.begin(), mesh_pool_.end(),
                                    [](const std::shared_ptr<Mesh> &mesh) { return mesh.use_count() == 1; }),
                     mesh_pool_.end());
}

}// namespace vox
