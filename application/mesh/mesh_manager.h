//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "base/singleton.h"
#include "mesh/model_mesh.h"

namespace vox {
class MeshManager : public Singleton<MeshManager> {
public:
    static MeshManager &get_singleton();

    static MeshManager *get_singleton_ptr();

    explicit MeshManager(MTL::Device &device);

    ~MeshManager() = default;

    std::shared_ptr<ModelMesh> load_model_mesh();

    void collect_garbage();

private:
    MTL::Device &device_;
    std::vector<std::shared_ptr<Mesh>> mesh_pool_;
};

template<>
inline MeshManager *Singleton<MeshManager>::ms_singleton{nullptr};
}// namespace vox
