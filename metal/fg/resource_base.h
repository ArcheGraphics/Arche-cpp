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
class RenderTaskBase;
class RenderTaskBuilder;

class ResourceBase {
public:
    explicit ResourceBase(const std::string &name, const RenderTaskBase *creator)
        : name_(name), creator_(creator), ref_count_(0) {
        static std::size_t id = 0;
        id_ = id++;
    }
    ResourceBase(const ResourceBase &that) = delete;
    ResourceBase(ResourceBase &&temp) = default;
    virtual ~ResourceBase() = default;
    ResourceBase &operator=(const ResourceBase &that) = delete;
    ResourceBase &operator=(ResourceBase &&temp) = default;

    std::size_t id() const {
        return id_;
    }

    const std::string &name() const {
        return name_;
    }
    void set_name(const std::string &name) {
        name_ = name;
    }

    bool transient() const {
        return creator_ != nullptr;
    }

protected:
    friend Framegraph;
    friend RenderTaskBuilder;

    virtual void realize() = 0;
    virtual void derealize() = 0;

    std::size_t id_;
    std::string name_;
    const RenderTaskBase *creator_;
    std::vector<const RenderTaskBase *> readers_;
    std::vector<const RenderTaskBase *> writers_;
    std::size_t ref_count_;// Computed through framegraph compilation.
};
}// namespace vox::fg