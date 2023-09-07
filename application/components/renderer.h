//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "math/bounding_box3.h"
#include "math/matrix4x4.h"
#include "ecs/component.h"
#include "rendering/render_element.h"
#include "shader/shader_data.h"
#include "base/update_flag.h"
#include "material/material.h"

namespace vox {
/**
 * Renderable component.
 */
class Renderer : public Component {
public:
    using MaterialPtr = std::shared_ptr<Material>;

    struct alignas(16) RendererData {
        Matrix4x4F local_mat;
        Matrix4x4F model_mat;
        Matrix4x4F normal_mat;
    };

    /** ShaderData related to renderer. */
    ShaderData shader_data_;
    /** Whether it is clipped by the frustum, needs to be turned on camera.enableFrustumCulling. */
    bool is_culled_ = false;

    /** Set whether the renderer to receive shadows. */
    bool receive_shadow_ = false;
    /** Set whether the renderer to cast shadows. */
    bool cast_shadow_ = false;

    /**
     * Material count.
     */
    size_t get_material_count();

    /**
     * The bounding volume of the renderer.
     */
    BoundingBox3F get_bounds();

    explicit Renderer(Entity *entity);

public:
    /**
     * Get the first instance material by index.
     * @remarks Calling this function for the first time after the material is set will create an instance material to
     * ensure that it is unique to the renderer.
     * @param index - Material index
     * @returns Instance material
     */
    MaterialPtr get_instance_material(size_t index = 0);

    /**
     * Get the first material by index.
     * @param index - Material index
     * @returns Material
     */
    MaterialPtr get_material(size_t index = 0);

    /**
     * Set the first material.
     * @param material - The first material
     */
    void set_material(const MaterialPtr &material);

    /**
     * Set material by index.
     * @param index - Material index
     * @param material - The material
     */
    void set_material(size_t index, const MaterialPtr &material);

    /**
     * Get all instance materials.
     * @remarks Calling this function for the first time after the material is set will create an instance material to
     * ensure that it is unique to the renderer.
     * @returns All instance materials
     */
    std::vector<MaterialPtr> get_instance_materials();

    /**
     * Get all materials.
     * @returns All materials
     */
    std::vector<MaterialPtr> get_materials();

    /**
     * Set all materials.
     * @param materials - All materials
     */
    void set_materials(const std::vector<MaterialPtr> &materials);

    static void push_primitive(const RenderElement &element,
                               std::vector<RenderElement> &opaque_queue,
                               std::vector<RenderElement> &alpha_test_queue,
                               std::vector<RenderElement> &transparent_queue);

    void set_distance_for_sort(float dist);

    [[nodiscard]] float get_distance_for_sort() const;

    void update_shader_data();

protected:
    void on_enable() override;

    void on_disable() override;

    virtual void render(std::vector<RenderElement> &opaque_queue,
                        std::vector<RenderElement> &alpha_test_queue,
                        std::vector<RenderElement> &transparent_queue) = 0;

    virtual void update_bounds(BoundingBox3F &world_bounds) {}

    virtual void update(float delta_time) {}

protected:
    MaterialPtr create_instance_material(const MaterialPtr &material, size_t index);

    std::vector<std::shared_ptr<Material>> materials_;

private:
    friend class ComponentsManager;

    float distance_for_sort_ = 0;
    ssize_t renderer_index_ = -1;

    RendererData renderer_data_;
    const std::string renderer_property_;

    std::unique_ptr<UpdateFlag> transform_change_flag_;
    BoundingBox3F bounds_ = BoundingBox3F();
    Matrix4x4F normal_matrix_ = Matrix4x4F();
    std::vector<bool> materials_instanced_;
};

}// namespace vox
