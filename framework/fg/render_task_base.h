//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace vox::fg {
class Framegraph;
class RenderTaskBuilder;
class ResourceBase;

class RenderTaskBase {
public:
    explicit RenderTaskBase(const std::string &name) : name_(name), cull_immune_(false), ref_count_(0) {
    }
    RenderTaskBase(const RenderTaskBase &that) = delete;
    RenderTaskBase(RenderTaskBase &&temp) = default;
    virtual ~RenderTaskBase() = default;
    RenderTaskBase &operator=(const RenderTaskBase &that) = delete;
    RenderTaskBase &operator=(RenderTaskBase &&temp) = default;

    const std::string &name() const {
        return name_;
    }
    void set_name(const std::string &name) {
        name_ = name;
    }

    bool cull_immune() const {
        return cull_immune_;
    }
    void set_cull_immune(const bool cull_immune) {
        cull_immune_ = cull_immune;
    }

protected:
    friend Framegraph;
    friend RenderTaskBuilder;

    virtual void setup(RenderTaskBuilder &builder) = 0;
    virtual void execute() const = 0;

    std::string name_;
    bool cull_immune_;
    std::vector<const ResourceBase *> creates_;
    std::vector<const ResourceBase *> reads_;
    std::vector<const ResourceBase *> writes_;
    std::size_t ref_count_;// Computed through framegraph compilation.
};
}// namespace vox::fg
