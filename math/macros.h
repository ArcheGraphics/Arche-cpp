//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define VOX_WINDOWS
#elif defined(__APPLE__)
#define VOX_APPLE
#ifndef VOX_IOS
#define VOX_MACOSX
#endif
#elif defined(linux) || defined(__linux__)
#define VOX_LINUX
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define VOX_DEBUG_MODE

#include <cassert>

#define VOX_ASSERT(x) assert(x)
#else
#define VOX_ASSERT(x)
#endif

#ifdef __cplusplus
#define VOX_NON_COPYABLE(ClassName)       \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;
#endif

#ifdef __cplusplus

#include <stdexcept>

#define VOX_THROW_INVALID_ARG_IF(expression)      \
    if (expression) {                             \
        throw std::invalid_argument(#expression); \
    }
#define VOX_THROW_INVALID_ARG_WITH_MESSAGE_IF(expression, message) \
    if (expression) {                                              \
        throw std::invalid_argument(message);                      \
    }
#endif

#if defined(VOX_WINDOWS) && defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#define VOX_DIAG_STR(s) #s
#define VOX_DIAG_JOINSTR(x, y) VOX_DIAG_STR(x##y)
#ifdef _MSC_VER
#define VOX_DIAG_DO_PRAGMA(x) __pragma(#x)
#define VOX_DIAG_PRAGMA(compiler, x) VOX_DIAG_DO_PRAGMA(warning(x))
#else
#define VOX_DIAG_DO_PRAGMA(x) _Pragma(#x)
#define VOX_DIAG_PRAGMA(compiler, x) VOX_DIAG_DO_PRAGMA(compiler diagnostic x)
#endif
#if defined(__clang__)
#define VOX_DISABLE_CLANG_WARNING(clang_option) \
    VOX_DIAG_PRAGMA(clang, push)                \
    VOX_DIAG_PRAGMA(clang, ignored VOX_DIAG_JOINSTR(-W, clang_option))
#define VOX_ENABLE_CLANG_WARNING(clang_option) VOX_DIAG_PRAGMA(clang, pop)
#define VOX_DISABLE_MSVC_WARNING(gcc_option)
#define VOX_ENABLE_MSVC_WARNING(gcc_option)
#define VOX_DISABLE_GCC_WARNING(gcc_option)
#define VOX_ENABLE_GCC_WARNING(gcc_option)
#elif defined(_MSC_VER)
#define VOX_DISABLE_CLANG_WARNING(gcc_option)
#define VOX_ENABLE_CLANG_WARNING(gcc_option)
#define VOX_DISABLE_MSVC_WARNING(msvc_errorcode) \
    VOX_DIAG_PRAGMA(msvc, push)                  \
    VOX_DIAG_DO_PRAGMA(warning(disable :##msvc_errorcode))
#define VOX_ENABLE_MSVC_WARNING(msvc_errorcode) VOX_DIAG_PRAGMA(msvc, pop)
#define VOX_DISABLE_GCC_WARNING(gcc_option)
#define VOX_ENABLE_GCC_WARNING(gcc_option)
#elif defined(__GNUC__)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#define VOX_DISABLE_CLANG_WARNING(gcc_option)
#define VOX_ENABLE_CLANG_WARNING(gcc_option)
#define VOX_DISABLE_MSVC_WARNING(gcc_option)
#define VOX_ENABLE_MSVC_WARNING(gcc_option)
#define VOX_DISABLE_GCC_WARNING(gcc_option) \
    VOX_DIAG_PRAGMA(GCC, push)              \
    VOX_DIAG_PRAGMA(GCC, ignored VOX_DIAG_JOINSTR(-W, gcc_option))
#define VOX_ENABLE_GCC_WARNING(gcc_option) VOX_DIAG_PRAGMA(GCC, pop)
#else
#define VOX_DISABLE_CLANG_WARNING(gcc_option)
#define VOX_ENABLE_CLANG_WARNING(gcc_option)
#define VOX_DISABLE_MSVC_WARNING(gcc_option)
#define VOX_ENABLE_MSVC_WARNING(gcc_option)
#define VOX_DISABLE_GCC_WARNING(gcc_option) VOX_DIAG_PRAGMA(GCC, ignored VOX_DIAG_JOINSTR(-W, gcc_option))
#define VOX_ENABLE_GCC_WARNING(gcc_option) VOX_DIAG_PRAGMA(GCC, warning VOX_DIAG_JOINSTR(-W, gcc_option))
#endif
#endif
