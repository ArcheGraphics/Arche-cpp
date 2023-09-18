//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <type_traits>

namespace vox::compute {

// Note: extra namespace qualifications are not allowed in GCC,
//  so we place the template implementation in the detail namespace.
namespace detail {
template<typename T>
struct is_stream_event_impl : std::false_type {};
}// namespace detail

#define VOX_MARK_STREAM_EVENT_TYPE(T) \
    template<>                          \
    struct vox::compute::detail::is_stream_event_impl<T> : std::true_type {};

template<typename T>
using is_stream_event = detail::is_stream_event_impl<std::remove_cvref_t<T>>;

template<typename T>
constexpr auto is_stream_event_v = is_stream_event<T>::value;

}// namespace vox::compute
