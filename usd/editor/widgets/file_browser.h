//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <string>
#include <vector>
#include <set>

namespace vox {
/// Draw a file browser ui which can be used anywhere, except that there is only one instance of the FileBrowser
/// for the application. There is no OK/Cancel button, or options, this is the responsibility of the caller to draw them.
void draw_file_browser(int gutterSize = 190);

/// Returns the current stored file browser path
std::string get_file_browser_file_path();

/// Returns the current stored file browser path, relative to root
std::string get_file_browser_file_path_relative_to(const std::string &root, bool unixify = false);

/// Make sure the file path has an extension, if not "ext" is append as an extension
void ensure_file_browser_default_extension(const std::string &ext);

/// Make sure the file extension is ext
void ensure_file_browser_extension(const std::string &ext);

/// Reset currently stored path in the file browser
void reset_file_browser_file_path();

/// Set the filebrowser directory to look at
void set_file_browser_directory(const std::string &directory);

/// Set the filebrowser directory to look at
void set_file_browser_file_path(const std::string &path);

/// Set the filebrowser directory to look at
std::string get_file_browser_directory();

/// Returns true if the current file browser path exists
bool file_path_exists();

/// Sets the valid extensions. The file browser will filter out any files with an invalid extension.
void set_valid_extensions(const std::vector<std::string> &extensions);

}// namespace vox