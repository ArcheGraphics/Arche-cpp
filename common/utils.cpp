//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "utils.h"

#include <stdexcept>
#include <sstream>

namespace vox {
std::string get_extension(const std::string &uri) {
    auto dot_pos = uri.find_last_of('.');
    if (dot_pos == std::string::npos) {
        throw std::runtime_error{"Uri has no extension"};
    }

    return uri.substr(dot_pos + 1);
}

std::string to_snake_case(const std::string &text) {
    std::stringstream result;

    for (const auto ch : text) {
        if (std::isalpha(ch)) {
            if (std::isspace(ch)) {
                result << "_";
            } else {
                if (std::isupper(ch)) {
                    result << "_";
                }

                result << static_cast<char>(std::tolower(ch));
            }
        } else {
            result << ch;
        }
    }

    return result.str();
}

}// namespace vox
