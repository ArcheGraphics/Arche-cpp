//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader/shader.h"
#include "shader/shader_data.h"
#include "shader/state/render_state.h"
#include "enums/render_queue_type.h"

namespace vox {
/**
 * Material.
 */
class Material {
public:
    /** Name. */
    std::string name;

    /** Render queue type. */
    RenderQueueType::Enum renderQueueType = RenderQueueType::Enum::OPAQUE;

    /** Shader used by the material. */
    std::string vertex_source;
    std::string fragment_source;

    /** Shader data. */
    ShaderData shader_data;

    /** Render state. */
    RenderState renderState = RenderState();

    explicit Material(MTL::Device &device, std::string name = "");

    Material(Material &&other) = default;

    virtual ~Material() = default;

protected:
    MTL::Device &device_;
};

}// namespace vox