//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_buffer.h"
#include "metal_texture.h"
#include "metal_stream.h"
#include "metal_event.h"
#include "metal_swapchain.h"
#include "metal_device.h"

// extensions

#include "metal_debug_capture.h"

namespace vox::compute::metal {

MetalDevice::MetalDevice(const DeviceConfig *config) noexcept
    : DeviceInterface{},
      _inqueue_buffer_limit{config == nullptr || config->inqueue_buffer_limit} {

    auto device_index = config == nullptr ? 0u : config->device_index;
    auto all_devices = MTL::CopyAllDevices();
    auto device_count = all_devices->count();
    ASSERT(device_index < device_count,
                 "Metal device index out of range.");
    _handle = all_devices->object<MTL::Device>(device_index)->retain();
    all_devices->release();

    ASSERT(_handle->supportsFamily(MTL::GPUFamilyMetal3),
                 "Metal device '{}' at index {} does not support Metal 3.",
                 _handle->name()->utf8String(), device_index);

    // TODO: load built-in kernels
    auto builtin_kernel_source = NS::String::alloc()->init(
        const_cast<char *>(luisa_metal_builtin_metal_builtin_kernels),
        sizeof(luisa_metal_builtin_metal_builtin_kernels),
        NS::UTF8StringEncoding, false);
    auto compile_options = MTL::CompileOptions::alloc()->init();
    compile_options->setFastMathEnabled(true);
    compile_options->setLanguageVersion(MTL::LanguageVersion3_0);
    compile_options->setLibraryType(MTL::LibraryTypeExecutable);
    NS::Error *error{nullptr};
    auto builtin_library = _handle->newLibrary(builtin_kernel_source, compile_options, &error);
    builtin_library->setLabel(MTLSTR("luisa_builtin"));

    builtin_kernel_source->release();
    compile_options->release();

    if (error != nullptr) {
        WARNING_WITH_LOCATION(
            "Failed to compile built-in Metal kernels: {}",
            error->localizedDescription()->utf8String());
    }
    error = nullptr;
    LUISA_ASSERT(builtin_library != nullptr,
                 "Failed to compile built-in Metal kernels.");

    // compute pipelines
    auto compute_pipeline_desc = MTL::ComputePipelineDescriptor::alloc()->init();
    compute_pipeline_desc->setThreadGroupSizeIsMultipleOfThreadExecutionWidth(true);
    auto create_builtin_compute_shader = [&](auto name, auto block_size) noexcept {
        compute_pipeline_desc->setMaxTotalThreadsPerThreadgroup(block_size);
        auto function_desc = MTL::FunctionDescriptor::alloc()->init();
        function_desc->setName(name);
        function_desc->setOptions(MTL::FunctionOptionCompileToBinary);
        auto function = builtin_library->newFunction(function_desc, &error);
        function_desc->release();
        if (error != nullptr) {
            LUISA_WARNING_WITH_LOCATION(
                "Failed to compile built-in Metal kernel '{}': {}",
                name->utf8String(), error->localizedDescription()->utf8String());
        }
        error = nullptr;
        LUISA_ASSERT(function != nullptr,
                     "Failed to compile built-in Metal kernel '{}'.",
                     name->utf8String());
        compute_pipeline_desc->setComputeFunction(function);
        auto pipeline = _handle->newComputePipelineState(
            compute_pipeline_desc, MTL::PipelineOptionNone, nullptr, &error);
        if (error != nullptr) {
            LUISA_WARNING_WITH_LOCATION(
                "Failed to compile built-in Metal kernel '{}': {}",
                name->utf8String(), error->localizedDescription()->utf8String());
        }
        error = nullptr;
        LUISA_ASSERT(pipeline != nullptr,
                     "Failed to compile built-in Metal kernel '{}'.",
                     name->utf8String());
        function->release();
        return pipeline;
    };
    _builtin_update_bindless_slots = create_builtin_compute_shader(
        MTLSTR("update_bindless_array"), update_bindless_slots_block_size);
    _builtin_update_accel_instances = create_builtin_compute_shader(
        MTLSTR("update_accel_instances"), update_accel_instances_block_size);
    _builtin_prepare_indirect_dispatches = create_builtin_compute_shader(
        MTLSTR("prepare_indirect_dispatches"), prepare_indirect_dispatches_block_size);
    compute_pipeline_desc->release();

    // render pipeline
    auto create_builtin_raster_shader = [&](auto name) noexcept {
        auto shader_desc = MTL::FunctionDescriptor::alloc()->init();
        shader_desc->setName(name);
        shader_desc->setOptions(MTL::FunctionOptionCompileToBinary);
        auto shader = builtin_library->newFunction(shader_desc, &error);
        shader_desc->release();
        if (error != nullptr) {
            LUISA_WARNING_WITH_LOCATION(
                "Failed to compile built-in Metal vertex shader '{}': {}",
                name->utf8String(), error->localizedDescription()->utf8String());
        }
        error = nullptr;
        LUISA_ASSERT(shader != nullptr,
                     "Failed to compile built-in Metal rasterization shader '{}'.",
                     name->utf8String());
        return shader;
    };
    auto builtin_swapchain_vertex_shader = create_builtin_raster_shader(MTLSTR("swapchain_vertex_shader"));
    auto builtin_swapchain_fragment_shader = create_builtin_raster_shader(MTLSTR("swapchain_fragment_shader"));

    auto render_pipeline_desc = MTL::RenderPipelineDescriptor::alloc()->init();
    render_pipeline_desc->setVertexFunction(builtin_swapchain_vertex_shader);
    render_pipeline_desc->setFragmentFunction(builtin_swapchain_fragment_shader);
    auto color_attachment = render_pipeline_desc->colorAttachments()->object(0u);
    color_attachment->setBlendingEnabled(false);
    auto create_builtin_present_shader = [&](auto format) noexcept {
        color_attachment->setPixelFormat(format);
        auto shader = _handle->newRenderPipelineState(
            render_pipeline_desc, MTL::PipelineOptionNone, nullptr, &error);
        if (error != nullptr) {
            WARNING_WITH_LOCATION(
                "Failed to compile built-in Metal kernel 'swapchain_fragment_shader': {}",
                error->localizedDescription()->utf8String());
        }
        error = nullptr;
        ASSERT(shader != nullptr,
                     "Failed to compile built-in Metal kernel 'swapchain_fragment_shader'.");
        return shader;
    };
    _builtin_swapchain_present_ldr = create_builtin_present_shader(MTL::PixelFormatBGRA8Unorm);
    _builtin_swapchain_present_hdr = create_builtin_present_shader(MTL::PixelFormatRGBA16Float);
    render_pipeline_desc->release();
    builtin_swapchain_vertex_shader->release();
    builtin_swapchain_fragment_shader->release();

    builtin_library->release();

    LOGI("Created Metal device '{}' at index {}.",
               _handle->name()->utf8String(), device_index);
}

MetalDevice::~MetalDevice() noexcept {
    _builtin_update_bindless_slots->release();
    _builtin_update_accel_instances->release();
    _builtin_prepare_indirect_dispatches->release();
    _builtin_swapchain_present_ldr->release();
    _builtin_swapchain_present_hdr->release();
    _handle->release();
}

void *MetalDevice::native_handle() const noexcept {
    return _handle;
}

uint MetalDevice::compute_warp_size() const noexcept {
    return _builtin_update_bindless_slots->threadExecutionWidth();
}

[[nodiscard]] inline auto create_device_buffer(MTL::Device *device, size_t element_stride, size_t element_count) noexcept {
    auto buffer_size = element_stride * element_count;
    auto buffer = new_with_allocator<MetalBuffer>(device, buffer_size);
    BufferCreationInfo info{};
    info.handle = reinterpret_cast<uint64_t>(buffer);
    info.native_handle = buffer->handle();
    info.element_stride = element_stride;
    info.total_size_bytes = buffer_size;
    return info;
}

BufferCreationInfo MetalDevice::create_buffer(size_t elem_count) noexcept {
    return with_autorelease_pool([=, this] {
        // special handling of the indirect dispatch buffer
        if (element == Type::of<IndirectKernelDispatch>()) {
            auto p = new_with_allocator<MetalIndirectDispatchBuffer>(_handle, elem_count);
            BufferCreationInfo info{};
            info.handle = reinterpret_cast<uint64_t>(p);
            info.native_handle = p->dispatch_buffer();
            info.element_stride = sizeof(MetalIndirectDispatchBuffer::Dispatch);
            info.total_size_bytes = p->dispatch_buffer()->length();
            return info;
        }
        if (element == Type::of<void>()) {
            return create_device_buffer(_handle, 1u, elem_count);
        }
        // normal buffer
        auto elem_size = MetalCodegenAST::type_size_bytes(element);
        return create_device_buffer(_handle, elem_size, elem_count);
    });
}

void MetalDevice::destroy_buffer(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto buffer = reinterpret_cast<MetalBufferBase *>(handle);
        delete_with_allocator(buffer);
    });
}

ResourceCreationInfo MetalDevice::create_texture(PixelFormat format, uint dimension,
                                                 uint width, uint height, uint depth, uint mipmap_levels,
                                                 bool allow_simultaneous_access) noexcept {
    return with_autorelease_pool([=, this] {
        auto texture = new_with_allocator<MetalTexture>(
            _handle, format, dimension, width, height, depth,
            mipmap_levels, allow_simultaneous_access);
        ResourceCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(texture);
        info.native_handle = texture->handle();
        return info;
    });
}

void MetalDevice::destroy_texture(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto texture = reinterpret_cast<MetalTexture *>(handle);
        delete_with_allocator(texture);
    });
}

ResourceCreationInfo MetalDevice::create_stream(StreamTag stream_tag) noexcept {
    return with_autorelease_pool([=, this] {
        auto stream = new_with_allocator<MetalStream>(
            _handle, _inqueue_buffer_limit ? 4u : 0u);
        ResourceCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(stream);
        info.native_handle = stream->queue();
        return info;
    });
}

void MetalDevice::destroy_stream(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto stream = reinterpret_cast<MetalStream *>(handle);
        delete_with_allocator(stream);
    });
}

void MetalDevice::synchronize_stream(uint64_t stream_handle) noexcept {
    with_autorelease_pool([=] {
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->synchronize();
    });
}

void MetalDevice::dispatch(uint64_t stream_handle, CommandList &&list) noexcept {
    with_autorelease_pool([stream_handle, &list] {
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->dispatch(std::move(list));
    });
}

SwapchainCreationInfo MetalDevice::create_swapchain(uint64_t window_handle, uint64_t stream_handle,
                                                    uint width, uint height, bool allow_hdr,
                                                    bool vsync, uint back_buffer_size) noexcept {
    return with_autorelease_pool([=, this] {
        auto swapchain = new_with_allocator<MetalSwapchain>(
            this, window_handle, width, height,
            allow_hdr, vsync, back_buffer_size);
        SwapchainCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(swapchain);
        info.native_handle = swapchain->layer();
        info.storage = swapchain->pixel_storage();
        return info;
    });
}

void MetalDevice::destroy_swap_chain(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto swpachain = reinterpret_cast<MetalSwapchain *>(handle);
        delete_with_allocator(swpachain);
    });
}

void MetalDevice::present_display_in_stream(uint64_t stream_handle, uint64_t swapchain_handle, uint64_t image_handle) noexcept {
    with_autorelease_pool([=] {
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        auto swapchain = reinterpret_cast<MetalSwapchain *>(swapchain_handle);
        auto image = reinterpret_cast<MetalTexture *>(image_handle);
        stream->present(swapchain, image);
    });
}

ShaderCreationInfo MetalDevice::load_shader(std::string_view name) noexcept {
    return with_autorelease_pool([=, this] {
        MetalShaderMetadata metadata{};
        auto pipeline = _compiler->load(name, metadata);
        LUISA_ASSERT(pipeline.entry && pipeline.indirect_entry,
                     "Failed to load Metal AOT shader '{}'.", name);
        LUISA_ASSERT(metadata.argument_types.size() == arg_types.size(),
                     "Argument count mismatch in Metal AOT "
                     "shader '{}': expected {}, but got {}.",
                     name, metadata.argument_types.size(), arg_types.size());
        for (auto i = 0u; i < arg_types.size(); i++) {
            LUISA_ASSERT(metadata.argument_types[i] == arg_types[i]->description(),
                         "Argument type mismatch in Metal AOT "
                         "shader '{}': expected {}, but got {}.",
                         name, metadata.argument_types[i],
                         arg_types[i]->description());
        }
        auto shader = new_with_allocator<MetalShader>(
            this, std::move(pipeline),
            std::move(metadata.argument_usages),
            luisa::vector<MetalShader::Argument>{},
            metadata.block_size);
        ShaderCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(shader);
        info.native_handle = shader->pso();
        info.block_size = metadata.block_size;
        return info;
    });
}

Usage MetalDevice::shader_argument_usage(uint64_t handle, size_t index) noexcept {
    auto shader = reinterpret_cast<MetalShader *>(handle);
    return shader->argument_usage(index);
}

void MetalDevice::destroy_shader(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto shader = reinterpret_cast<MetalShader *>(handle);
        luisa::delete_with_allocator(shader);
    });
}

ResourceCreationInfo MetalDevice::create_event() noexcept {
    return with_autorelease_pool([=, this] {
        auto event = new_with_allocator<MetalEvent>(_handle);
        ResourceCreationInfo info{};
        info.handle = reinterpret_cast<uint64_t>(event);
        info.native_handle = event->handle();
        return info;
    });
}

void MetalDevice::destroy_event(uint64_t handle) noexcept {
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        delete_with_allocator(event);
    });
}

void MetalDevice::signal_event(uint64_t handle, uint64_t stream_handle, uint64_t value) noexcept {
    // TODO: fence not implemented
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->signal(event, value);
    });
}

void MetalDevice::wait_event(uint64_t handle, uint64_t stream_handle, uint64_t value) noexcept {
    // TODO: fence not implemented
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        auto stream = reinterpret_cast<MetalStream *>(stream_handle);
        stream->wait(event, value);
    });
}

void MetalDevice::synchronize_event(uint64_t handle, uint64_t value) noexcept {
    // TODO: fence not implemented
    with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        event->synchronize(value);
    });
}

bool MetalDevice::is_event_completed(uint64_t handle, uint64_t value) const noexcept {
    // TODO: fence not implemented
    return with_autorelease_pool([=] {
        auto event = reinterpret_cast<MetalEvent *>(handle);
        return event->is_completed(value);
    });
}

std::string MetalDevice::query(std::string_view property) noexcept {
    WARNING_WITH_LOCATION("Device property \"{}\" is not supported on Metal.", property);
    return {};
}

DeviceExtension *MetalDevice::extension(std::string_view name) noexcept {
    return with_autorelease_pool([=, this]() noexcept -> DeviceExtension * {
        if (name == DebugCaptureExt::name) {
            std::scoped_lock lock{_ext_mutex};
            if (!_debug_capture_ext) { _debug_capture_ext = std::make_unique<MetalDebugCaptureExt>(this); }
            return _debug_capture_ext.get();
        }
        WARNING_WITH_LOCATION("Device extension \"{}\" is not supported on Metal.", name);
        return nullptr;
    });
}

void MetalDevice::set_name(Resource::Tag resource_tag,
                           uint64_t resource_handle, std::string_view name) noexcept {

    with_autorelease_pool([=] {
        switch (resource_tag) {
            case Resource::Tag::BUFFER: {
                auto buffer = reinterpret_cast<MetalBufferBase *>(resource_handle);
                buffer->set_name(name);
                break;
            }
            case Resource::Tag::TEXTURE: {
                auto texture = reinterpret_cast<MetalTexture *>(resource_handle);
                texture->set_name(name);
                break;
            }
            case Resource::Tag::STREAM: {
                auto stream = reinterpret_cast<MetalStream *>(resource_handle);
                stream->set_name(name);
                break;
            }
            case Resource::Tag::EVENT: {
                auto event = reinterpret_cast<MetalEvent *>(resource_handle);
                event->set_name(name);
                break;
            }
            case Resource::Tag::SHADER: {
                auto shader = reinterpret_cast<MetalShader *>(resource_handle);
                shader->set_name(name);
                break;
            }
            case Resource::Tag::SWAP_CHAIN: {
                auto swapchain = reinterpret_cast<MetalSwapchain *>(resource_handle);
                swapchain->set_name(name);
                break;
            }
        }
    });
}

}// namespace vox::compute::metal
