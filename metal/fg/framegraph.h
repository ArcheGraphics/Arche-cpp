//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <fstream>
#include <iterator>
#include <memory>
#include <stack>
#include <string>
#include <type_traits>
#include <vector>

#include "fg/render_task.h"
#include "fg/render_task_builder.h"
#include "fg/resource.h"

namespace vox::fg {
class Framegraph {
public:
    Framegraph() = default;
    Framegraph(const Framegraph &that) = delete;
    Framegraph(Framegraph &&temp) = default;
    virtual ~Framegraph() = default;
    Framegraph &operator=(const Framegraph &that) = delete;
    Framegraph &operator=(Framegraph &&temp) = default;

    template<typename data_type, typename... argument_types>
    RenderTask<data_type> *add_render_task(argument_types &&...arguments) {
        render_tasks_.emplace_back(std::make_unique<RenderTask<data_type>>(arguments...));
        auto render_task = render_tasks_.back().get();

        RenderTaskBuilder builder(this, render_task);
        render_task->setup(builder);

        return static_cast<fg::RenderTask<data_type> *>(render_task);
    }

    template<typename description_type, typename actual_type>
    Resource<description_type, actual_type> *add_retained_resource(const std::string &name, const description_type &description, actual_type *actual = nullptr) {
        resources_.emplace_back(std::make_unique<Resource<description_type, actual_type>>(name, description, actual));
        return static_cast<Resource<description_type, actual_type> *>(resources_.back().get());
    }

    void compile();

    void execute() const;

    void clear();

    void export_graphviz(const std::string &filepath);

protected:
    friend RenderTaskBuilder;

    struct step {
        RenderTaskBase *render_task;
        std::vector<ResourceBase *> realized_resources;
        std::vector<ResourceBase *> derealized_resources;
    };

    std::vector<std::unique_ptr<RenderTaskBase>> render_tasks_;
    std::vector<std::unique_ptr<ResourceBase>> resources_;
    std::vector<step> timeline_;// Computed through framegraph compilation.
};

template<typename resource_type, typename description_type>
resource_type *RenderTaskBuilder::create(const std::string &name, const description_type &description) {
    static_assert(std::is_same<typename resource_type::description_type, description_type>::value, "Description does not match the resource.");
    framegraph_->resources_.emplace_back(std::make_unique<resource_type>(name, render_task_, description));
    const auto resource = framegraph_->resources_.back().get();
    render_task_->creates_.push_back(resource);
    return static_cast<resource_type *>(resource);
}

template<typename resource_type>
resource_type *RenderTaskBuilder::read(resource_type *resource) {
    resource->readers_.push_back(render_task_);
    render_task_->reads_.push_back(resource);
    return resource;
}

template<typename resource_type>
resource_type *RenderTaskBuilder::write(resource_type *resource) {
    resource->writers_.push_back(render_task_);
    render_task_->writes_.push_back(resource);
    return resource;
}
}// namespace vox::fg