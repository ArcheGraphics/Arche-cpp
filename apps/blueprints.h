//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace vox {
// Blueprints class
//   - traverse the blueprint root locations looking for layers organised hierarchically on the disk.
//   - keep the hierarchy structure, names and paths of the blueprint layers for the whole application time.

class Blueprints {
public:
    static Blueprints &get_instance();

    // Calling SetBlueprintsLocations will reset the stored data and traverse
    // the root locations looking for blueprints
    void set_blueprints_locations(const std::vector<std::string> &locations);

    const std::vector<std::string> &get_sub_folders(std::string folder);

    const std::vector<std::pair<std::string, std::string>> &get_items(std::string folder);

private:
    std::unordered_map<std::string, std::vector<std::string>> _subFolders;
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> _items;
    Blueprints() = default;
    ~Blueprints() = default;
};
}// namespace vox