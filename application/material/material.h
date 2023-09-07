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
    /** Shader used by the material. */
    Shader *shader;
    /** Render queue type. */
    RenderQueueType::Enum renderQueueType = RenderQueueType::Enum::OPAQUE;
    /** Shader data. */
    ShaderData shaderData = ShaderData();
    /** Render state. */
    RenderState renderState = RenderState();

    /**
     * Create a material instance.
     * @param shader - Shader used by the material
     */
    explicit Material(Shader *shader);
};

}// namespace vox