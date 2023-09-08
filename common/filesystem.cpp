//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cassert>
#include "common/filesystem.h"

namespace vox::fs {
namespace path {
const std::unordered_map<Type, std::string> relative_paths = {
    {Type::Assets, "assets/"},
    {Type::Shaders, "shaders/"},
    {Type::Storage, "output/"},
    {Type::Screenshots, "output/images/"},
    {Type::Logs, "output/logs/"},
};

std::string get(const Type type, const std::string &file) {
    assert(relative_paths.size() == Type::TotalRelativePathTypes && "Not all paths are defined in filesystem, please check that each enum is specified");

    // Check for relative paths
    auto it = relative_paths.find(type);

    if (relative_paths.size() < Type::TotalRelativePathTypes) {
        throw std::runtime_error("Platform hasn't initialized the paths correctly");
    } else if (it == relative_paths.end()) {
        throw std::runtime_error("Path enum doesn't exist, or wasn't specified in the path map");
    } else if (it->second.empty()) {
        throw std::runtime_error("Path was found, but it is empty");
    }

    auto path = it->second;

    if (!is_directory(path)) {
        create_path("", it->second);
    }

    return path + file;
}
}// namespace path

bool is_directory(const std::string &path) {
    struct stat info {};
    if (stat(path.c_str(), &info) != 0) {
        return false;
    } else if (info.st_mode & S_IFDIR) {
        return true;
    } else {
        return false;
    }
}

bool is_file(const std::string &filename) {
    std::ifstream f(filename.c_str());
    return !f.fail();
}

void create_directory(const std::string &path) {
    if (!is_directory(path)) {
        mkdir(path.c_str(), 0777);
    }
}

void create_path(const std::string &root, const std::string &path) {
    for (auto it = path.begin(); it != path.end(); ++it) {
        it = std::find(it, path.end(), '/');
        create_directory(root + std::string(path.begin(), it));
    }
}

std::string read_text_file(const std::string &filename) {
    std::vector<std::string> data;

    std::ifstream file;

    file.open(filename, std::ios::in);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    return std::string{(std::istreambuf_iterator<char>(file)),
                       (std::istreambuf_iterator<char>())};
}

template<typename T>
std::vector<T> read_binary_file(const std::string &filename, const uint32_t count) {
    std::vector<T> data;

    std::ifstream file;

    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    uint64_t read_count = count;
    if (count == 0) {
        file.seekg(0, std::ios::end);
        read_count = static_cast<uint64_t>(file.tellg());
        file.seekg(0, std::ios::beg);
    }

    data.resize(static_cast<size_t>(read_count) / sizeof(T) * sizeof(uint8_t));
    file.read(reinterpret_cast<char *>(data.data()), read_count);
    file.close();

    return data;
}

static void write_binary_file(const std::vector<uint8_t> &data, const std::string &filename, const uint32_t count) {
    std::ofstream file;

    file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    uint64_t write_count = count;
    if (count == 0) {
        write_count = data.size();
    }

    file.write(reinterpret_cast<const char *>(data.data()), write_count);
    file.close();
}

std::vector<uint8_t> read_asset(const std::string &filename, const uint32_t count) {
    return read_binary_file<uint8_t>(path::get(path::Type::Assets) + filename, count);
}

std::string read_shader(const std::string &filename) {
    return read_text_file(path::get(path::Type::Shaders) + filename);
}

std::vector<uint32_t> read_spv(const std::string &filename) {
    return read_binary_file<uint32_t>(path::get(path::Type::Shaders) + filename, 0);
}

std::vector<uint8_t> read_temp(const std::string &filename, const uint32_t count) {
    return read_binary_file<uint8_t>(path::get(path::Type::Temp) + filename, count);
}

void write_temp(const std::vector<uint8_t> &data, const std::string &filename, const uint32_t count) {
    write_binary_file(data, path::get(path::Type::Temp) + filename, count);
}

}// namespace vox::fs
