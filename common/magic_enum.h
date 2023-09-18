//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <type_traits>
#include <optional>
#include "common/basic_traits.h"

#define MAGIC_ENUM_USING_ALIAS_OPTIONAL using std::optional;
#define MAGIC_ENUM_USING_ALIAS_STRING using std::string;
#define MAGIC_ENUM_USING_ALIAS_STRING_VIEW using std::string_view;
#include <magic_enum.hpp>

namespace vox {

template<typename T>
    requires std::is_enum_v<T>
[[nodiscard]] inline auto to_string(T e) noexcept {
    return magic_enum::enum_name(e);
}

}// namespace vox

#define LUISA_MAGIC_ENUM_RANGE(E, E_MIN, E_MAX)                                    \
    namespace magic_enum::customize {                                              \
    template<>                                                                     \
    struct enum_range<E> {                                                         \
        static_assert(std::to_underlying(E::E_MIN) == static_cast<int>(E::E_MIN)); \
        static_assert(std::to_underlying(E::E_MAX) == static_cast<int>(E::E_MAX)); \
        static constexpr int min = static_cast<int>(E::E_MIN);                     \
        static constexpr int max = static_cast<int>(E::E_MAX);                     \
    };                                                                             \
    }
