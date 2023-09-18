//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "runtime/rhi/resource.h"
#include "runtime/device.h"
#include "common/logging.h"

namespace vox::compute {

Resource::Resource(Resource &&rhs) noexcept
    : _device{std::move(rhs._device)},
      _info{rhs._info},
      _tag{rhs._tag} { rhs._info.invalidate(); }

Resource::Resource(DeviceInterface *device,
                   Resource::Tag tag,
                   const ResourceCreationInfo &info) noexcept
    : _device{device->shared_from_this()},
      _info{info}, _tag{tag} {}

void Resource::set_name(std::string_view name) const noexcept {
    _device->set_name(_tag, _info.handle, name);
}

void Resource::_check_same_derived_types(const Resource &lhs,
                                         const Resource &rhs) noexcept {
    if (lhs && rhs) {
        ASSERT(lhs._tag == rhs._tag, "Cannot move resources of different types.");
    }
}

void Resource::_error_invalid() noexcept {
    ERROR_WITH_LOCATION("Invalid resource.");
}

}// namespace vox::compute
