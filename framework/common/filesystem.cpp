//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "filesystem.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

#include <glog/logging.h>

#include "engine.h"

namespace vox {
namespace fs {
namespace path {
const std::unordered_map<Type, std::string> relative_paths = {{Type::Assets, "../assets/"},
    {Type::Shaders, "WGSL/"},
    {Type::Storage, "output/"},
    {Type::Screenshots, "output/images/"},
    {Type::Logs, "output/logs/"},
    {Type::Graphs, "output/graphs/"}};

const std::string get(const Type type, const std::string &file) {
    assert(relative_paths.size() == Type::TotalRelativePathTypes
           && "Not all paths are defined in filesystem, please check that each enum is specified");
    
    // Check for special cases first
    if (type == Type::WorkingDir) {
        return Engine::externalStorageDirectory();
    } else if (type == Type::Temp) {
        return Engine::tempDirectory();
    }
    
    // Check for relative paths
    auto it = relative_paths.find(type);
    
    if (relative_paths.size() < Type::TotalRelativePathTypes) {
        throw std::runtime_error("Platform hasn't initialized the paths correctly");
    } else if (it == relative_paths.end()) {
        throw std::runtime_error("Path enum doesn't exist, or wasn't specified in the path map");
    } else if (it->second.empty()) {
        throw std::runtime_error("Path was found, but it is empty");
    }
    
    auto path = Engine::externalStorageDirectory() + it->second;
    
    if (!isDirectory(path)) {
        createPath(Engine::externalStorageDirectory(), it->second);
    }
    
    return path + file;
}
}        // namespace path

bool isDirectory(const std::string &path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    } else if (info.st_mode & S_IFDIR) {
        return true;
    } else {
        return false;
    }
}

bool isFile(const std::string &filename) {
    std::ifstream f(filename.c_str());
    return !f.fail();
}

void createPath(const std::string &root, const std::string &path) {
    for (auto it = path.begin(); it != path.end(); ++it) {
        it = std::find(it, path.end(), '/');
        createDirectory(root + std::string(path.begin(), it));
    }
}

std::string readTextFile(const std::string &filename) {
    std::vector<std::string> data;
    
    std::ifstream file;
    
    file.open(filename, std::ios::in);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    return std::string{(std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())};
}

std::vector<uint8_t> readBinaryFile(const std::string &filename, const uint32_t count) {
    std::vector<uint8_t> data;
    
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
    
    data.resize(static_cast<size_t>(read_count));
    file.read(reinterpret_cast<char *>(data.data()), read_count);
    file.close();
    
    return data;
}

static void writeBinaryFile(const std::vector<uint8_t> &data,
                            const std::string &filename, const uint32_t count) {
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

std::vector<uint8_t> readAsset(const std::string &filename, const uint32_t count) {
    return readBinaryFile(path::get(path::Type::Assets) + filename, count);
}

std::string readShader(const std::string &filename) {
    return readTextFile(path::get(path::Type::Assets) + path::get(path::Type::Shaders) + filename);
}

std::vector<uint8_t> readShaderBinary(const std::string &filename) {
    return readBinaryFile(path::get(path::Type::Shaders) + filename, 0);
}

std::vector<uint8_t> readTemp(const std::string &filename, const uint32_t count) {
    return readBinaryFile(path::get(path::Type::Temp) + filename, count);
}

void writeTemp(const std::vector<uint8_t> &data, const std::string &filename, const uint32_t count) {
    writeBinaryFile(data, path::get(path::Type::Temp) + filename, count);
}

void writeImage(const uint8_t *data, const std::string &filename,
                const uint32_t width, const uint32_t height,
                const uint32_t components, const uint32_t row_stride) {
    stbi_write_png((path::get(path::Type::Screenshots) + filename + ".png").c_str(),
                   width, height, components, data, row_stride);
}

bool writeJson(nlohmann::json &data, const std::string &filename) {
    std::stringstream json;
    
    try {
        // Whitespace needed as last character is overwritten on android causing the json to be corrupt
        json << data << " ";
    }
    catch (std::exception &e) {
        // JSON dump errors
        LOG(ERROR) << e.what() << std::endl;
        return false;
    }
    
    if (!nlohmann::json::accept(json.str())) {
        LOG(ERROR) << "Invalid JSON string" << std::endl;;
        return false;
    }
    
    std::ofstream out_stream;
    out_stream.open(fs::path::get(vox::fs::path::Type::Graphs) + filename, std::ios::out | std::ios::trunc);
    
    if (out_stream.good()) {
        out_stream << json.str();
    } else {
        LOG(ERROR) << "Could not load JSON file " << filename;
        return false;
    }
    
    out_stream.close();
    return true;
}

std::string extraExtension(const std::string &uri) {
    auto dot_pos = uri.find_last_of('.');
    if (dot_pos == std::string::npos) {
        throw std::runtime_error{"Uri has no extension"};
    }
    
    return uri.substr(dot_pos + 1);
}

}        // namespace fs
}        // namespace vox
