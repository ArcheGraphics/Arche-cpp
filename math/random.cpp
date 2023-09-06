//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "math/random.h"
#include <cassert>

namespace vox {
Random::Random() : Random(0) {}

Random::Random(uint32_t seed) : engine_(std::default_random_engine()) { setSeed(seed); }

void Random::setSeed(uint32_t seed) {
    engine_.seed(seed);
    seed_ = seed;
}

float Random::randomf(float min, float max) const {
    assert(min <= max);
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(engine_);
}

ssize_t Random::random(ssize_t min, ssize_t max) const {
    assert(min <= max);
    if (min == max) {
        return min;
    }
    std::uniform_int_distribution<ssize_t> distribution(min, max);
    return distribution(engine_);
}

float Random::randomBinomial(float max) const { return randomf(0.0f, max) - randomf(0.0f, max); }

}  // namespace vox
