//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "base/layer.h"
#include "components/transform.h"

//#include "scene_graph/components/transform.h"

namespace vox {
class Component;

/// @brief A leaf of the tree structure which can have children and a single parent.
class Entity {
public:
    /** The name of entity. */
    std::string name;
    /** The layer the entity belongs to. */
    int layer = Layer::Layer0;
    /** Transform component. */
    Transform *transform;

    /**
     * Create a entity.
     */
    Entity(std::string name = "");

    /**
     * Whether to activate locally.
     */
    bool isActive();

    void setIsActive(bool value);

    /**
     * Whether it is active in the hierarchy.
     */
    bool isActiveInHierarchy();

    /**
     * The parent entity.
     */
    Entity *parent();

    /**
     * Number of the children entities
     */
    size_t childCount();

    /**
     * The scene the entity belongs to.
     */
    Scene *scene();

    /**
     * The children entities
     */
    const std::vector<EntityPtr> children() const;

    /**
     * Add component based on the component type.
     * @returns    The component which has been added.
     */
    template<typename T>
    T *addComponent() {
        // ComponentsDependencies._addCheck(this, type);
        auto component = std::make_unique<T>(this);
        T *componentPtr = component.get();
        _components.emplace_back(std::move(component));
        if (_isActiveInHierarchy) {
            componentPtr->_setActive(true);
        }
        return componentPtr;
    }

    /**
     * Get component which match the type.
     * @returns    The first component which match type.
     */
    template<typename T>
    T *getComponent() {
        for (size_t i = 0; i < _components.size(); i++) {
            T *component = dynamic_cast<T *>(_components[i].get());
            if (component) {
                return component;
            }
        }
        return nullptr;
    }

    /**
     * Get components which match the type.
     * @returns    The components which match type.
     */
    template<typename T>
    std::vector<T *> getComponents() {
        std::vector<T *> results;
        for (size_t i = 0; i < _components.size(); i++) {
            T *component = dynamic_cast<T *>(_components[i].get());
            if (component) {
                results.push_back(component);
            }
        }
        return results;
    }

    /**
     * Get the components which match the type of the entity and it's children.
     * @returns    The components collection which match the type.
     */
    template<typename T>
    std::vector<T *> getComponentsIncludeChildren() {
        std::vector<T *> results;
        _getComponentsInChildren<T>(results);
        return results;
    }

    /**
     * Add child entity.
     * @param child - The child entity which want to be added.
     */
    void addChild(EntityPtr child);

    /**
     * Remove child entity.
     * @param child - The child entity which want to be removed.
     */
    void removeChild(EntityPtr child);

    /**
     * Find child entity by index.
     * @param index - The index of the child entity.
     * @returns    The component which be found.
     */
    EntityPtr getChild(int index);

    /**
     * Find child entity by name.
     * @param name - The name of the entity which want to be found.
     * @returns The component which be found.
     */
    EntityPtr findByName(const std::string &name);

    /**
     * Find the entity by path.
     * @param path - The path fo the entity eg: /entity.
     * @returns The component which be found.
     */
    EntityPtr findByPath(const std::string &path);

    /**
     * Create child entity.
     * @param name - The child entity's name.
     * @returns The child entity.
     */
    EntityPtr createChild(const std::string &name = "");

    /**
     * Clear children entities.
     */
    void clearChildren();

    /**
     * Clone
     * @returns Cloned entity.
     */
    EntityPtr clone();

    /**
     * Destroy self.
     */
    void destroy();

public:
    std::vector<Script *> scripts();

private:
    friend class ComponentsManager;

    friend class Component;

    friend class Transform;

    friend class Script;

    friend class Scene;

    void _removeComponent(Component *component);

    void _addScript(Script *script);

    void _removeScript(Script *script);

    Entity *_removeFromParent();

    void _processActive();

    void _processInActive();

    template<typename T>
    void _getComponentsInChildren(std::vector<T *> &results) {
        for (size_t i = 0; i < _components.size(); i++) {
            T *component = dynamic_cast<T *>(_components[i].get());
            if (component) {
                results.push_back(component);
            }
        }
        for (size_t i = 0; i < _children.size(); i++) {
            _children[i]->_getComponentsInChildren(results);
        }
    }

    void _setActiveComponents(bool isActive);

    void _setActiveInHierarchy(std::vector<Component *> &activeChangedComponents);

    void _setInActiveInHierarchy(std::vector<Component *> &activeChangedComponents);

    void _setTransformDirty();

    static EntityPtr _findChildByName(Entity *root, const std::string &name);

    static void _traverseSetOwnerScene(Entity *entity, Scene *scene);

    bool _isActiveInHierarchy = false;
    std::vector<std::unique_ptr<Component>> _components{};
    std::vector<Script *> _scripts{};
    std::vector<EntityPtr> _children{};
    Scene *_scene = nullptr;
    bool _isRoot = false;
    bool _isActive = true;

    Entity *_parent = nullptr;
    std::vector<Component *> _activeChangedComponents{};

    std::unique_ptr<UpdateFlag> _inverseWorldMatFlag = nullptr;
};

}// namespace vox
