//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#define LOGGER_FORMAT "[%^%l%$] %v"
#define PROJECT_NAME "VkCompute"

// Mainly for IDEs
#ifndef ROOT_PATH_SIZE
#define ROOT_PATH_SIZE 0
#endif

#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

#define LOGI(...) spdlog::info(__VA_ARGS__);
#define LOGW(...) spdlog::warn(__VA_ARGS__);
#define LOGE(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOGD(...) spdlog::debug(__VA_ARGS__);

class Logger;

// Returns the logger that discards all messages.
Logger &get_null_logger();

// Returns the logger that writes messages to std::clog.
Logger &get_error_logger();

// A simple logger that writes messages to an output stream if not null.
//
// This logger uses standard I/O so it should only be used in binaries.
class Logger {
public:
    friend Logger &get_null_logger();
    friend Logger &get_error_logger();

    template<class T>
    Logger &operator<<(const T &content);

    // Disable copy construction and assignment
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

private:
    enum class Type {
        NONE,
        ERROR,
    };
    Type type_;
    explicit Logger(Type type) : type_(type) {}
};

template<class T>
Logger &Logger::operator<<(const T &content) {
    switch (type_) {
        case Type::NONE:
            break;
        case Type::ERROR:
            spdlog::error(content);
            break;
    }
    return *this;
}
