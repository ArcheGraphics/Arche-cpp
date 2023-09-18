//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_callback_context.h"

namespace vox::compute::metal {

Pool<FunctionCallbackContext, true, true> &FunctionCallbackContext::_object_pool() noexcept {
    static Pool<FunctionCallbackContext, true, true> pool;
    return pool;
}

void FunctionCallbackContext::recycle() noexcept {
    _function();
    _object_pool().destroy(this);
}

}// namespace vox::compute::metal
