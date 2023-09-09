//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
template<typename T>
inline std::shared_ptr<T> make_shared(T *ptr) {
    return std::shared_ptr<T>(ptr, [](T *obj) {
        obj->release();
    });
}

}// namespace vox