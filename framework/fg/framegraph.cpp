//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "fg/framegraph.h"

namespace vox::fg {
void Framegraph::compile() {
    // Reference counting.
    for (auto &render_task : render_tasks_)
        render_task->ref_count_ = render_task->creates_.size() + render_task->writes_.size();
    for (auto &resource : resources_)
        resource->ref_count_ = resource->readers_.size();

    // Culling via flood fill from unreferenced resources.
    std::stack<ResourceBase *> unreferenced_resources;
    for (auto &resource : resources_)
        if (resource->ref_count_ == 0 && resource->transient())
            unreferenced_resources.push(resource.get());
    while (!unreferenced_resources.empty()) {
        auto unreferenced_resource = unreferenced_resources.top();
        unreferenced_resources.pop();

        auto creator = const_cast<RenderTaskBase *>(unreferenced_resource->creator_);
        if (creator->ref_count_ > 0)
            creator->ref_count_--;
        if (creator->ref_count_ == 0 && !creator->cull_immune()) {
            for (auto iteratee : creator->reads_) {
                auto read_resource = const_cast<ResourceBase *>(iteratee);
                if (read_resource->ref_count_ > 0)
                    read_resource->ref_count_--;
                if (read_resource->ref_count_ == 0 && read_resource->transient())
                    unreferenced_resources.push(read_resource);
            }
        }

        for (auto c_writer : unreferenced_resource->writers_) {
            auto writer = const_cast<RenderTaskBase *>(c_writer);
            if (writer->ref_count_ > 0)
                writer->ref_count_--;
            if (writer->ref_count_ == 0 && !writer->cull_immune()) {
                for (auto iteratee : writer->reads_) {
                    auto read_resource = const_cast<ResourceBase *>(iteratee);
                    if (read_resource->ref_count_ > 0)
                        read_resource->ref_count_--;
                    if (read_resource->ref_count_ == 0 && read_resource->transient())
                        unreferenced_resources.push(read_resource);
                }
            }
        }
    }

    // Timeline computation.
    timeline_.clear();
    for (auto &render_task : render_tasks_) {
        if (render_task->ref_count_ == 0 && !render_task->cull_immune())
            continue;

        std::vector<ResourceBase *> realized_resources, derealized_resources;

        for (auto resource : render_task->creates_) {
            realized_resources.push_back(const_cast<ResourceBase *>(resource));
            if (resource->readers_.empty() && resource->writers_.empty())
                derealized_resources.push_back(const_cast<ResourceBase *>(resource));
        }

        auto reads_writes = render_task->reads_;
        reads_writes.insert(reads_writes.end(), render_task->writes_.begin(), render_task->writes_.end());
        for (auto resource : reads_writes) {
            if (!resource->transient())
                continue;

            auto valid = false;
            std::size_t last_index;
            if (!resource->readers_.empty()) {
                auto last_reader = std::find_if(
                    render_tasks_.begin(),
                    render_tasks_.end(),
                    [&resource](const std::unique_ptr<RenderTaskBase> &iteratee) {
                        return iteratee.get() == resource->readers_.back();
                    });
                if (last_reader != render_tasks_.end()) {
                    valid = true;
                    last_index = std::distance(render_tasks_.begin(), last_reader);
                }
            }
            if (!resource->writers_.empty()) {
                auto last_writer = std::find_if(
                    render_tasks_.begin(),
                    render_tasks_.end(),
                    [&resource](const std::unique_ptr<RenderTaskBase> &iteratee) {
                        return iteratee.get() == resource->writers_.back();
                    });
                if (last_writer != render_tasks_.end()) {
                    valid = true;
                    last_index = std::max(last_index, std::size_t(std::distance(render_tasks_.begin(), last_writer)));
                }
            }

            if (valid && render_tasks_[last_index] == render_task)
                derealized_resources.push_back(const_cast<ResourceBase *>(resource));
        }

        timeline_.push_back(step{render_task.get(), realized_resources, derealized_resources});
    }
}

void Framegraph::execute() const {
    for (auto &step : timeline_) {
        for (auto resource : step.realized_resources) resource->realize();
        step.render_task->execute();
        for (auto resource : step.derealized_resources) resource->derealize();
    }
}

void Framegraph::clear() {
    render_tasks_.clear();
    resources_.clear();
}

void Framegraph::export_graphviz(const std::string &filepath) {
    std::ofstream stream(filepath);
    stream << "digraph framegraph \n{\n";

    stream << "rankdir = LR\n";
    stream << "bgcolor = black\n\n";
    stream << "node [shape=rectangle, fontname=\"helvetica\", fontsize=12]\n\n";

    for (auto &render_task : render_tasks_)
        stream << "\"" << render_task->name() << "\" [label=\"" << render_task->name() << "\\nRefs: " << render_task->ref_count_ << "\", style=filled, fillcolor=darkorange]\n";
    stream << "\n";

    for (auto &resource : resources_)
        stream << "\"" << resource->name() << "\" [label=\"" << resource->name() << "\\nRefs: " << resource->ref_count_ << "\\nID: " << resource->id() << "\", style=filled, fillcolor= " << (resource->transient() ? "skyblue" : "steelblue") << "]\n";
    stream << "\n";

    for (auto &render_task : render_tasks_) {
        stream << "\"" << render_task->name() << "\" -> { ";
        for (auto &resource : render_task->creates_)
            stream << "\"" << resource->name() << "\" ";
        stream << "} [color=seagreen]\n";

        stream << "\"" << render_task->name() << "\" -> { ";
        for (auto &resource : render_task->writes_)
            stream << "\"" << resource->name() << "\" ";
        stream << "} [color=gold]\n";
    }
    stream << "\n";

    for (auto &resource : resources_) {
        stream << "\"" << resource->name() << "\" -> { ";
        for (auto &render_task : resource->readers_)
            stream << "\"" << render_task->name() << "\" ";
        stream << "} [color=firebrick]\n";
    }
    stream << "}";
}

}// namespace vox::fg