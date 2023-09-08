//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <utility>

namespace vox::compute {
class fp16;

enum class DataType {
    fp16,
    fp32,
    i8,
    i32,
};

template<DataType DT>
struct DataTypeTraits {
    // Specialize this trait to support new data types.
    using storage_type = void;
    using runtime_type = void;
    static constexpr char name[] = "";
};

template<>
struct DataTypeTraits<DataType::fp16> {
    using storage_type = uint16_t;
    using runtime_type = fp16;
    static constexpr char name[] = "fp16";
};

template<>
struct DataTypeTraits<DataType::fp32> {
    using storage_type = float;
    using runtime_type = float;
    static constexpr char name[] = "fp32";
};

template<>
struct DataTypeTraits<DataType::i8> {
    using storage_type = int8_t;
    using runtime_type = int8_t;
    static constexpr char name[] = "i8";
};

template<>
struct DataTypeTraits<DataType::i32> {
    using storage_type = int32_t;
    using runtime_type = int32_t;
    static constexpr char name[] = "i32";
};

/// Invokes the |fn| functor with a DataTypeTraits object matching |data_type|,
/// followed by the remaining arguments |args|. This is useful when converting
/// runtime data_type back to types available at the compilation time. Compared
/// to ad-hoc switch statements, this helper makes it easier to *statically*
/// make sure that all data types were handled.
template<typename Fn, typename... Args>
constexpr auto invoke_with_traits(DataType data_type, Fn &&fn, Args &&...args) {
    switch (data_type) {
        case DataType::fp16:
            return fn(DataTypeTraits<DataType::fp16>{}, std::forward<Args>(args)...);
        case DataType::fp32:
            return fn(DataTypeTraits<DataType::fp32>{}, std::forward<Args>(args)...);
        case DataType::i8:
            return fn(DataTypeTraits<DataType::i8>{}, std::forward<Args>(args)...);
        case DataType::i32:
            return fn(DataTypeTraits<DataType::i32>{}, std::forward<Args>(args)...);
    }
}

constexpr std::size_t get_size(DataType data_type) {
    return invoke_with_traits(data_type, [](auto traits) {
        return sizeof(typename decltype(traits)::storage_type);
    });
}

constexpr const char *get_name(DataType data_type) {
    return invoke_with_traits(data_type,
                              [](auto traits) { return decltype(traits)::name; });
}

// Class to emulate half float on CPU.
class fp16 {
public:
    explicit fp16(uint16_t v) : value_(v) {}
    explicit fp16(float x) { from_float(x); }

    fp16 &operator=(const float &x) {
        from_float(x);
        return *this;
    }
    fp16 &operator=(const int &x) {
        from_float(static_cast<float>(x));
        return *this;
    }
    fp16 &operator+=(const fp16 &x) {
        from_float(to_float() + x.to_float());
        return *this;
    }
    fp16 operator*(const fp16 &rhs) const {
        return fp16(to_float() * rhs.to_float());
    }
    bool operator==(const fp16 &rhs) const { return value_ == rhs.value_; }

    explicit operator float() const { return to_float(); }
    explicit operator uint16_t() const { return get_value(); }

    void from_float(float x);
    [[nodiscard]] float to_float() const;
    [[nodiscard]] uint16_t get_value() const { return value_; }

    friend std::ostream &operator<<(std::ostream &os, const fp16 &value) {
        return os << value.to_float();
    }

private:
    uint16_t value_{};
};

}// namespace vox::compute
