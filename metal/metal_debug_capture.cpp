//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "common/magic_enum.h"
#include "metal_device.h"
#include "metal_stream.h"
#include "metal_debug_capture.h"

namespace vox::compute::metal {

class MetalDebugCaptureScope : public concepts::Noncopyable {

private:
    MTL::CaptureScope *_scope;
    MTL::CaptureDescriptor *_descriptor;

public:
    MetalDebugCaptureScope(MTL::CaptureDescriptor *descriptor,
                           MTL::CaptureScope *scope) noexcept
        : _scope{scope}, _descriptor{descriptor} {}

    ~MetalDebugCaptureScope() noexcept {
        if (_scope) { _scope->endScope(); }
        _scope->release();
        _descriptor->release();
    }

    void begin_capture() const noexcept {
        auto manager = MTL::CaptureManager::sharedCaptureManager();
        NS::Error *error = nullptr;
        manager->startCapture(_descriptor, &error);
        if (error) {
            WARNING_WITH_LOCATION(
                "Failed to start debug capture: {}.",
                error->localizedDescription()->utf8String());
        }
    }

    void mark_begin() const noexcept {
        _scope->beginScope();
    }

    void mark_end() const noexcept {
        _scope->endScope();
    }
};

MetalDebugCaptureExt::MetalDebugCaptureExt(MetalDevice *device) noexcept
    : _device{device->handle()} {}

MetalDebugCaptureExt::~MetalDebugCaptureExt() noexcept = default;

namespace detail {

template<typename Object>
[[nodiscard]] inline auto create_capture_scope(std::string_view label,
                                               const compute::DebugCaptureOption &option,
                                               Object object) noexcept {
    auto desc = MTL::CaptureDescriptor::alloc()->init();
    switch (option.output) {
        case compute::DebugCaptureOption::Output::DEVELOPER_TOOLS:
            desc->setDestination(MTL::CaptureDestinationDeveloperTools);
            break;
        case compute::DebugCaptureOption::Output::GPU_TRACE_DOCUMENT:
            desc->setDestination(MTL::CaptureDestinationGPUTraceDocument);
            break;
        default:
            WARNING_WITH_LOCATION(
                "Unsupported debug capture output: {}.",
                vox::to_string(option.output));
    }
    if (!option.file_name.empty()) {
        auto file_name = NS::String::alloc()->init(
            const_cast<char *>(option.file_name.data()),
            option.file_name.size(),
            NS::UTF8StringEncoding, false);
        desc->setOutputURL(NS::URL::fileURLWithPath(file_name));
        file_name->release();
    } else if (desc->destination() == MTL::CaptureDestinationGPUTraceDocument) {
        auto name = label.empty() ? "metal.gputrace" : fmt::format("{}.gputrace", label);
        WARNING_WITH_LOCATION(
            "Debug capture output file name is empty. "
            "GPU trace document will be saved to '{}'.",
            name);
        auto mtl_name = NS::String::alloc()->init(
            name.data(), name.size(), NS::UTF8StringEncoding, false);
        desc->setOutputURL(NS::URL::fileURLWithPath(mtl_name));
        mtl_name->release();
    }

    auto scope = MTL::CaptureManager::sharedCaptureManager()->newCaptureScope(object);
    if (!label.empty()) {
        auto mtl_label = NS::String::alloc()->init(
            const_cast<char *>(label.data()), label.size(),
            NS::UTF8StringEncoding, false);
        scope->setLabel(mtl_label);
        mtl_label->release();
    }
    desc->setCaptureObject(scope);
    return new MetalDebugCaptureScope(desc, scope);
}

}// namespace detail

uint64_t MetalDebugCaptureExt::create_device_capture_scope(std::string_view label,
                                                           const compute::DebugCaptureOption &option) const noexcept {
    return with_autorelease_pool([&] {
        auto scope = detail::create_capture_scope(label, option, _device);
        return reinterpret_cast<uint64_t>(scope);
    });
}

uint64_t MetalDebugCaptureExt::create_stream_capture_scope(uint64_t stream_handle,
                                                           std::string_view label,
                                                           const compute::DebugCaptureOption &option) const noexcept {
    return with_autorelease_pool([&] {
        auto stream = reinterpret_cast<MetalStream *>(stream_handle)->queue();
        auto scope = detail::create_capture_scope(label, option, stream);
        return reinterpret_cast<uint64_t>(scope);
    });
}

void MetalDebugCaptureExt::destroy_capture_scope(uint64_t handle) const noexcept {
    with_autorelease_pool([&] {
        auto scope = reinterpret_cast<MetalDebugCaptureScope *>(handle);
        delete scope;
    });
}

void MetalDebugCaptureExt::start_debug_capture(uint64_t handle) const noexcept {
    with_autorelease_pool([&] {
        auto scope = reinterpret_cast<MetalDebugCaptureScope *>(handle);
        scope->begin_capture();
    });
}

void MetalDebugCaptureExt::stop_debug_capture() const noexcept {
    with_autorelease_pool([&] {
        auto manager = MTL::CaptureManager::sharedCaptureManager();
        manager->stopCapture();
    });
}

void MetalDebugCaptureExt::mark_scope_begin(uint64_t handle) const noexcept {
    with_autorelease_pool([&] {
        auto scope = reinterpret_cast<MetalDebugCaptureScope *>(handle);
        scope->mark_begin();
    });
}

void MetalDebugCaptureExt::mark_scope_end(uint64_t handle) const noexcept {
    with_autorelease_pool([&] {
        auto scope = reinterpret_cast<MetalDebugCaptureScope *>(handle);
        scope->mark_end();
    });
}

}// namespace vox::compute::metal
