//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "graphics_application.h"
#include "engine.h"
#include "shader/shader.h"

#include <glog/logging.h>
#include "metal_helpers.h"

namespace vox {
MetalApplication::MetalApplication() {
    Shader::create("unlit", "vertex_unlit", "fragment_unlit");
    Shader::create("blinn-phong", "vertex_blinn_phong", "fragment_blinn_phong");
    Shader::create("particle-shader", "vertex_particle", "fragment_particle");
    Shader::create("pbr", "vertex_blinn_phong", "fragment_pbr");
    Shader::create("pbr-specular", "vertex_blinn_phong", "fragment_pbr");
    Shader::create("skybox", "vertex_skybox", "fragment_skybox");

    // MARK: - experimental
    Shader::create("shadow-map", "vertex_shadow_map", "fragment_shadow_map");
    Shader::create("shadow", "vertex_shadow_map", "fragment_shadow");
    Shader::create("background-texture", "vertex_background_texture", "fragment_background_texture");

    Shader::create("experimental", "vertex_experimental", "fragment_experimental");
}

bool MetalApplication::prepare(Engine &engine) {
    if (!Application::prepare(engine)) {
        return false;
    }

    LOG(INFO) << "Initializing Metal Application";

    _device = CLONE_METAL_CUSTOM_DELETER(MTL::Device, MTL::CreateSystemDefaultDevice());
    printf("Selected Device: %s\n", _device->name()->cString(NS::StringEncoding::UTF8StringEncoding));

    _library = makeShaderLibrary();

    _commandQueue = CLONE_METAL_CUSTOM_DELETER(MTL::CommandQueue, _device->newCommandQueue());

    _renderContext = engine.createRenderContext(*_device);
    return true;
}

void MetalApplication::update(float delta_time) {
    _renderContext->nextDrawable();
}

bool MetalApplication::resize(uint32_t win_width, uint32_t win_height,
                              uint32_t fb_width, uint32_t fb_height) {
    Application::resize(win_width, win_height, fb_width, fb_height);
    _renderContext->resize(fb_width, fb_height);
    return true;
}

void MetalApplication::inputEvent(const InputEvent &inputEvent) {
}

void MetalApplication::finish() {
}

std::shared_ptr<MTL::Library> MetalApplication::makeShaderLibrary() {
    NS::Error *error;
    CFURLRef libraryURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("vox.shader"), CFSTR("metallib"), nullptr);

    std::shared_ptr<MTL::Library> shaderLibrary =
        CLONE_METAL_CUSTOM_DELETER(MTL::Library, _device->newLibrary((NS::URL *)libraryURL, &error));

    if (error != nullptr) {
        LOG(ERROR) << "Error: could not load Metal shader library: "
                   << error->description()->cString(NS::StringEncoding::UTF8StringEncoding) << std::endl;
    }

    CFRelease(libraryURL);
    return shaderLibrary;
}

}// namespace vox
