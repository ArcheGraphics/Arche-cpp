//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

/// File browser
/// This is a first quick and dirty implementation,
/// it should be improved to avoid using globals,
/// split the ui and filesystem code, remove the polling timer, etc.

#include <iostream>
#include <functional>
#include <ctime>
#include <chrono>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <filesystem>

#include "file_browser.h"
#include "base/imgui_helpers.h"
#include "base/constants.h"

namespace clk = std::chrono;
using DrivesListT = std::vector<std::pair<std::string, std::string>>;
/*
Posix specific implementation
*/
static DrivesListT drivesList;

inline void localtime_(struct tm *result, const time_t *timep) { localtime_r(timep, result); }

namespace vox {
namespace {
/// Browser returned file path, not thread safe
std::string filePath;
bool fileExists = false;
std::vector<std::string> validExts;
std::string lineEditBuffer;
std::filesystem::path displayedDirectory = std::filesystem::current_path();
}// namespace

void set_valid_extensions(const std::vector<std::string> &extensions) { validExts = extensions; }

// Using a timer to avoid querying the filesystem at every frame
static void every_second(const std::function<void()> &deferedFunction) {
    static auto last = clk::steady_clock::now();
    auto now = clk::steady_clock::now();
    if ((now - last).count() > 1e9) {
        deferedFunction();
        last = now;
    }
}

inline void convert_to_directory(const std::filesystem::path &path, std::string &directory) {
    if (!path.empty() && path == path.root_name()) {// this is a drive
        auto path_ = path / path.root_directory();
        directory = path_.string();
    } else {
        directory = path.string();
    }
}

constexpr char preferred_separator_char_windows = '\\';
constexpr char preferred_separator_char_unix = '/';
static constexpr char preferred_separator_char = preferred_separator_char_unix;

static bool draw_navigation_bar(std::filesystem::path &displayedDirectory) {
    // Split the path navigator ??
    std::string lineEditBuffer;
    ScopedStyleColor style(ImGuiCol_Button, ImVec4(ColorTransparent));
    const std::string &directoryPath = displayedDirectory.string();
    std::string::size_type pos = 0;
    std::string::size_type len = 0;
    len = directoryPath.find(preferred_separator_char, pos);

    while (len != std::string::npos) {
        if (pos == 0 && !drivesList.empty()) {
            if (ImGui::Button(ICON_FA_HDD)) {
                ImGui::OpenPopup("driveslist");
            }
            if (ImGui::BeginPopup("driveslist")) {
                for (const auto &driveInfo : drivesList) {
                    const std::string &driveLetter = driveInfo.first;
                    const std::string &driveVolumeName = driveInfo.second;
                    const std::string driveText = driveVolumeName + " (" + driveLetter + ")";
                    if (ImGui::Selectable(driveText.c_str(), false)) {
                        lineEditBuffer = driveLetter + preferred_separator_char;
                        displayedDirectory = std::filesystem::path(lineEditBuffer);
                        ImGui::EndPopup();
                        return true;
                    }
                }
                ImGui::EndPopup();
            }
            ImGui::SameLine();
        }
        const std::string dirLabel = directoryPath.substr(pos, len - pos);
        if (ImGui::Button(dirLabel.empty() ? "###emptydirlabel" : dirLabel.c_str())) {
            lineEditBuffer = directoryPath.substr(0, len) + preferred_separator_char;
            displayedDirectory = std::filesystem::path(lineEditBuffer);
            return true;
        }
        pos = len + 1;
        len = directoryPath.find(preferred_separator_char, pos);
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
    }
    len = directoryPath.size();
    const std::string dirLabel = directoryPath.substr(pos, len - pos);
    ImGui::Button(dirLabel.empty() ? "###emptydir" : dirLabel.c_str());
    return false;
}

inline static bool draw_refresh_button() {
    ScopedStyleColor style(ImGuiCol_Button, ImVec4(ColorTransparent));
    if (ImGui::Button(ICON_FA_SYNC_ALT)) {
        return true;
    }
    return false;
}

static inline bool file_name_starts_with_dot(const std::filesystem::path &path) {
    const std::string &filename = path.filename().string();
    return !filename.empty() && filename[0] == '.';
}

static bool should_be_displayed(const std::filesystem::directory_entry &p) {
    const auto &filename = p.path().filename();
    const auto startsWithDot = file_name_starts_with_dot(p.path());
    bool endsWithValidExt = true;
    if (!validExts.empty()) {
        endsWithValidExt = false;
        for (const auto &ext : validExts) {// we could stop the loop when the extension is found
            endsWithValidExt |= filename.extension() == ext;
        }
    }

    try {
        const bool isDirectory = std::filesystem::is_directory(p);
        return !startsWithDot && (isDirectory || endsWithValidExt) && !std::filesystem::is_symlink(p);
    } catch (std::filesystem::filesystem_error &) {
        return false;
    }
}

// Compare function for sorting directories before files
static bool compare_directory_then_file(const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b) {
    if (std::filesystem::is_directory(a) == std::filesystem::is_directory(b)) {
        return a < b;
    } else {
        return std::filesystem::is_directory(a) > std::filesystem::is_directory(b);
    }
}

static void draw_file_size(uintmax_t fileSize) {
    static const char *format[6] = {"%juB", "%juK", "%juM", "%juG", "%juT", "%juP"};
    constexpr int nbFormat = sizeof(format) / sizeof(const char *);
    int i = 0;
    while (fileSize / 1024 > 0 && i < nbFormat - 1) {
        fileSize /= 1024;
        i++;
    }
    ImGui::Text(format[i], fileSize);
}

void draw_file_browser(int gutterSize) {
    static std::filesystem::path displayedFileName;
    static std::vector<std::filesystem::directory_entry> directoryContent;
    static std::filesystem::path directoryContentPath;
    static bool mustUpdateDirectoryContent = true;
    static bool mustUpdateChosenFileName = false;

    // Parse the line buffer containing the user input and try to make sense of it
    auto ParseLineBufferEdit = [&]() {
        auto path = std::filesystem::path(lineEditBuffer);
        if (path != path.root_name() && std::filesystem::is_directory(path)) {
            displayedDirectory = path;
            mustUpdateDirectoryContent = true;
            lineEditBuffer = "";
            displayedFileName = "";
            mustUpdateChosenFileName = true;
        } else if (path.parent_path() != path.root_name() && std::filesystem::is_directory(path.parent_path())) {
            displayedDirectory = path.parent_path();
            mustUpdateDirectoryContent = true;
            lineEditBuffer = path.filename().string();
            displayedFileName = path.filename();
            mustUpdateChosenFileName = true;
        } else {
            if (path.filename() != path) {
                displayedFileName = path.filename();
                mustUpdateChosenFileName = true;
                lineEditBuffer = path.filename().string();
            } else {
                displayedFileName = path;
                mustUpdateChosenFileName = true;
            }
        }
    };

    // Update the list of entries for the chosen directory
    auto UpdateDirectoryContent = [&]() {
        directoryContent.clear();
        for (auto &item : std::filesystem::directory_iterator(displayedDirectory, std::filesystem::directory_options::skip_permission_denied)) {
            if (should_be_displayed(item)) {
                directoryContent.push_back(item);
            }
        }
        std::sort(directoryContent.begin(), directoryContent.end(), compare_directory_then_file);
        mustUpdateDirectoryContent = false;
        directoryContentPath = displayedDirectory;
    };

    if (mustUpdateChosenFileName) {
        if (!displayedDirectory.empty() && !displayedFileName.empty() && std::filesystem::exists(displayedDirectory) &&
            std::filesystem::is_directory(displayedDirectory)) {
            const auto path_ = displayedDirectory / displayedFileName;
            filePath = path_.string();
        } else {
            filePath = "";
        }
        fileExists = std::filesystem::exists(filePath);
        mustUpdateChosenFileName = false;
    }

    // We scan the line buffer edit every second, no need to do it at every frame
    every_second(ParseLineBufferEdit);
    mustUpdateDirectoryContent |= directoryContentPath != displayedDirectory;
    mustUpdateDirectoryContent |= draw_refresh_button();
    ImGui::SameLine();
    mustUpdateDirectoryContent |= draw_navigation_bar(displayedDirectory);

    if (mustUpdateDirectoryContent) {
        UpdateDirectoryContent();
    }

    // Get window size
    ImGuiWindow *currentWindow = ImGui::GetCurrentWindow();
    // TODO: 190 should be computed or passed in parameter as there might be other widget taking height
    ImVec2 sizeArg(0, currentWindow->Size[1] - gutterSize);
    ImGui::PushItemWidth(-1);// List takes the full size
    if (ImGui::BeginListBox("##FileList", sizeArg)) {
        constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("Files", 4, tableFlags)) {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Filename", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Date modified", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();
            int i = 0;
            ImGui::PushID("direntries");
            for (const auto &dirEntry : directoryContent) {
                const bool isDirectory = dirEntry.is_directory();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(i++);
                // makes the line selectable, and when selected copy the path
                // to the line edit buffer
                if (ImGui::Selectable("", false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                    if (isDirectory) {
                        displayedDirectory = dirEntry.path();
                        mustUpdateDirectoryContent = true;
                    } else {
                        displayedFileName = dirEntry.path();
                        lineEditBuffer = dirEntry.path().string();
                        mustUpdateChosenFileName = true;
                    }
                }
                ImGui::PopID();
                ImGui::SameLine();
                if (isDirectory) {
                    ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), "%s ", ICON_FA_FOLDER);
                    ImGui::TableSetColumnIndex(1);// the following line is allocating string for each directory/files, this is BAD
                    ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "%s", dirEntry.path().filename().string().c_str());
                } else {
                    ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1.0), "%s ", ICON_FA_FILE);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%s", dirEntry.path().filename().string().c_str());
                }
                ImGui::TableSetColumnIndex(2);
                //                try {
                //                    const auto lastModified = last_write_time(dirEntry.path());
                //                    const time_t cftime = decltype(lastModified)::clock::to_time_t(lastModified);
                //                    struct tm lt;// Convert to local time
                //                    localtime_(&lt, &cftime);
                //                    ImGui::Text("%04d/%02d/%02d %02d:%02d", 1900 + lt.tm_year, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min);
                //                } catch (const std::exception &) {
                //                    ImGui::Text("Error reading file");
                //                }
                ImGui::TableSetColumnIndex(3);
                draw_file_size(dirEntry.file_size());
            }
            ImGui::PopID();// direntries
            ImGui::EndTable();
        }
        ImGui::EndListBox();
    }

    ImGui::PopItemWidth();
    ImGui::InputText("File name", &lineEditBuffer);
}

bool file_path_exists() { return fileExists; }

std::string get_file_browser_file_path() { return filePath; }

std::string get_file_browser_directory() { return displayedDirectory.string(); }

std::string get_file_browser_file_path_relative_to(const std::string &root, bool unixify) {
    std::filesystem::path rootPath(root);
    const std::filesystem::path originalPath(filePath);
    if (rootPath.is_absolute() && originalPath.is_absolute()) {
        rootPath.remove_filename();
        const std::filesystem::path relativePath = std::filesystem::relative(originalPath, rootPath);
        auto relativePathStr = relativePath.string();
        if (unixify) {
            std::replace(relativePathStr.begin(), relativePathStr.end(), preferred_separator_char_windows,
                         preferred_separator_char_unix);
        }
        return relativePathStr;
    }
    return filePath;
}

void ensure_file_browser_default_extension(const std::string &ext) {
    std::filesystem::path path(filePath);
    if (!path.empty() && !path.has_extension()) {
        path.replace_extension(ext);
        filePath = path.generic_string();
        fileExists = std::filesystem::exists(filePath);
    }
}

void ensure_file_browser_extension(const std::string &ext) {
    std::filesystem::path path(filePath);
    if (!path.empty()) {
        path.replace_extension(ext);
        filePath = path.generic_string();
        fileExists = std::filesystem::exists(filePath);
    }
}

void reset_file_browser_file_path() {
    filePath = "";
    lineEditBuffer = "";
}

void set_file_browser_directory(const std::string &directory) {
    if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
        displayedDirectory = directory;
    }
}

void set_file_browser_file_path(const std::string &path) {
    lineEditBuffer = path;
}

}// namespace vox