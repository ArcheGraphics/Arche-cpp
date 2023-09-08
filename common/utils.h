//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "common/filesystem.h"

namespace vox {
/**
 * @brief Extracts the extension from an uri
 * @param uri An uniform Resource Identifier
 * @return The extension
 */
std::string get_extension(const std::string &uri);

/**
 * @param name String to convert to snake case
 * @return a snake case version of the string
 */
std::string to_snake_case(const std::string &name);

}// namespace vox
