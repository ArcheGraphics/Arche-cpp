//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "common/pool.h"

namespace vox::compute::metal {

struct MetalCallbackContext {
    virtual void recycle() noexcept = 0;
    virtual ~MetalCallbackContext() noexcept = default;
};

class FunctionCallbackContext : public MetalCallbackContext {

private:
    std::function<void()> _function;

private:
    using Self = FunctionCallbackContext;
    [[nodiscard]] static Pool<Self, true, true> &_object_pool() noexcept;

public:
    template<typename F>
    explicit FunctionCallbackContext(F &&f) noexcept
        : _function{std::forward<F>(f)} {}
    ~FunctionCallbackContext() noexcept override = default;

    FunctionCallbackContext(FunctionCallbackContext &&) = default;
    FunctionCallbackContext(const FunctionCallbackContext &) = delete;

    template<typename F>
    [[nodiscard]] static auto create(F &&f) noexcept {
        return _object_pool().create(std::forward<F>(f));
    }

    void recycle() noexcept override;
};

}// namespace vox::compute::metal
