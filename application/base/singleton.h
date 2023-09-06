//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef singleton_h
#define singleton_h

#include <cassert>

namespace vox {
/*
 * Template class for creating single-instance global classes.
 */
template<typename T>
class Singleton {
private:
    /** @brief Explicit private copy constructor. This is a forbidden operation.*/
    Singleton(const Singleton<T> &);

    /** @brief Private operator= . This is a forbidden operation. */
    Singleton &operator=(const Singleton<T> &);

protected:

    static T *msSingleton;

public:
    Singleton(void) {
        assert(!msSingleton);
        msSingleton = static_cast<T *>(this);
    }

    ~Singleton(void) {
        assert(msSingleton);
        msSingleton = 0;
    }

    static T &getSingleton(void) {
        assert(msSingleton);
        return (*msSingleton);
    }

    static T *getSingletonPtr(void) {
        return msSingleton;
    }
};

}// namespace vox
#endif /* singleton_h */
