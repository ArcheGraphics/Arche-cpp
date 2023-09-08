//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "logging.h"

Logger &get_null_logger() {
    static Logger logger(Logger::Type::NONE);
    return logger;
}

Logger &get_error_logger() {
    static Logger logger(Logger::Type::ERROR);
    return logger;
}