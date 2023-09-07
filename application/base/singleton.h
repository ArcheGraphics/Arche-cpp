//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>

namespace vox {
/*
 * Template class for creating single-instance global classes.
 */
template<typename T>
class Singleton {
public:
    /** @brief Explicit private copy constructor. This is a forbidden operation.*/
    Singleton(const Singleton<T> &) = delete;

    /** @brief Private operator= . This is a forbidden operation. */
    Singleton &operator=(const Singleton<T> &) = delete;

protected:
    static T *ms_singleton;

public:
    Singleton() {
        assert(!ms_singleton);
        ms_singleton = static_cast<T *>(this);
    }

    ~Singleton() {
        assert(ms_singleton);
        ms_singleton = 0;
    }

    static T &get_singleton() {
        assert(ms_singleton);
        return (*ms_singleton);
    }

    static T *get_singleton_ptr() { return ms_singleton; }
};

}// namespace vox