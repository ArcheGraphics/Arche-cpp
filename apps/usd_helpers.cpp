//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "usd_helpers.h"
#include <iostream>
#include <iomanip>

#include <pxr/usd/sdf/fileFormat.h>

namespace vox {
std::string find_next_available_token_string(std::string prefix) {
    // Find number in the prefix
    size_t end = prefix.size() - 1;
    while (end > 0 && std::isdigit(prefix[end])) {
        end--;
    }
    size_t padding = prefix.size() - 1 - end;
    const std::string number = prefix.substr(end + 1, padding);
    auto value = !number.empty() ? std::stoi(number) : 0;
    std::ostringstream newName;
    padding = padding == 0 ? 4 : padding;// 4: default padding
    do {
        value += 1;
        newName.seekp(0, std::ios_base::beg);// rewind
        newName << prefix.substr(0, end + 1) << std::setfill('0') << std::setw(padding) << value;
        // Looking for existing token with the same name.
        // There might be a better solution here
    } while (TfToken::Find(newName.str()) != TfToken());
    return newName.str();
}

std::vector<std::string> get_usd_valid_extensions() {
    const auto usdExtensions = SdfFileFormat::FindAllFileFormatExtensions();
    std::vector<std::string> validExtensions;
    auto addDot = [](const std::string &str) { return "." + str; };
    std::transform(usdExtensions.cbegin(), usdExtensions.cend(), std::back_inserter(validExtensions), addDot);
    return validExtensions;
}

}// namespace vox