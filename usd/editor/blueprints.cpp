//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "blueprints.h"
#include <iostream>
#include <stack>
#include <pxr/usd/sdf/fileFormat.h>

#include <filesystem>

PXR_NAMESPACE_USING_DIRECTIVE

namespace vox {
void Blueprints::set_blueprints_locations(const std::vector<std::string> &locations) {
    std::cout << "Reading blueprints" << std::endl;
    const std::set<std::string> allUsdExt = SdfFileFormat::FindAllFileFormatExtensions();
    std::stack<std::string> folders;
    std::stack<std::string> paths;
    for (const auto &loc : locations) {
        paths.push(loc);
        folders.push("");
    }
    _subFolders.clear();
    _items.clear();
    // TODO: we should measure the time it takes to read all the blueprints as this can affect the application startup time
    // Also this code could be ran in another thread as it is not essential to have the
    // blueprint informations at startup time
    // -> We might just want to read a json file containing a list of file/
    // if there is a json file; otherwise read the dir ??
    while (!paths.empty()) {
        if (!std::filesystem::exists(paths.top())) {
            std::cerr << "unable to find blueprint path " << paths.top() << std::endl;
            paths.pop();
            continue;
        }
        try {
            std::filesystem::is_directory(paths.top());
        } catch (std::filesystem::filesystem_error &) {
            std::cerr << "unable to read directory " << paths.top() << std::endl;
            paths.pop();
            continue;
        }
        const auto path = std::filesystem::directory_iterator(paths.top());
        paths.pop();
        const auto folder = folders.top();
        folders.pop();
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (std::filesystem::is_directory(entry)) {
                paths.push(entry.path().generic_string());
                std::string folderStem = std::prev(entry.path().end())->generic_string();
                if (!folderStem.empty()) {
                    folderStem[0] = std::toupper(folderStem[0]);
                    std::string subFolderName = folder + "/" + folderStem;
                    folders.push(subFolderName);
                    _subFolders[folder].push_back(subFolderName);
                }
            } else if (std::filesystem::is_regular_file(entry.path())) {
                std::string layerPath = entry.path().generic_string();
                const auto ext = SdfFileFormat::GetFileExtension(layerPath);
                if (allUsdExt.find(ext) != allUsdExt.end()) {
                    std::string itemName = entry.path().stem().generic_string();
                    if (!itemName.empty()) {
                        itemName[0] = std::toupper(itemName[0]);
                        _items[folder].push_back(std::make_pair(itemName, layerPath));
                    }
                }
            }
        }
    }
    std::cout << "Blueprints ready" << std::endl;
}

Blueprints &Blueprints::get_instance() {
    static Blueprints instance;
    return instance;
}

const std::vector<std::string> &Blueprints::get_sub_folders(std::string folder) { return _subFolders[folder]; }

const std::vector<std::pair<std::string, std::string>> &Blueprints::get_items(std::string folder) { return _items[folder]; }

}// namespace vox