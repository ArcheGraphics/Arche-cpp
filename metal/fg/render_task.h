//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>
#include <string>

#include "fg/render_task_base.h"

namespace vox::fg {
class RenderTaskBuilder;

template<typename data_type_>
class RenderTask : public RenderTaskBase {
public:
    using data_type = data_type_;

    explicit RenderTask(
        const std::string &name,
        const std::function<void(data_type &, RenderTaskBuilder &)> &setup,
        const std::function<void(const data_type &)> &execute) : RenderTaskBase(name), setup_(setup), execute_(execute) {
    }
    RenderTask(const RenderTask &that) = delete;
    RenderTask(RenderTask &&temp) = default;
    ~RenderTask() override = default;
    RenderTask &operator=(const RenderTask &that) = delete;
    RenderTask &operator=(RenderTask &&temp) = default;

    const data_type &data() const {
        return data_;
    }

protected:
    void setup(RenderTaskBuilder &builder) override {
        setup_(data_, builder);
    }
    void execute() const override {
        execute_(data_);
    }

    data_type data_;
    const std::function<void(data_type &, RenderTaskBuilder &)> setup_;
    const std::function<void(const data_type &)> execute_;
};
}// namespace vox::fg