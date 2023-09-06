//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <random>

#include "math/algorithm.h"

namespace vox {
class Random {
public:
    Random();

    Random(uint32_t seed);

    void setSeed(uint32_t seed);

    float randomf(float min = 0.0f, float max = 1.0f) const;

    ssize_t random(ssize_t min = 0, ssize_t max = RAND_MAX) const;

    bool fithyFifthy() const;

    float randomBinomial(float max = 1.0f) const;

    template <typename I>
    I randomElement(I begin, I end) const {
        const int kN = distance(begin, end);
        next(begin, random(0, kN - 1));
        return begin;
    }

    uint32_t seed() const;

private:
    uint32_t seed_{};
    mutable std::default_random_engine engine_;
};

inline bool Random::fithyFifthy() const { return randomf() >= 0.5f; }

inline uint32_t Random::seed() const { return seed_; }

}  // namespace vox
