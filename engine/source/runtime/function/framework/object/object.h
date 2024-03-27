#pragma once

#include "runtime/function/framework/framework_common.h"

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace BJTUGE {

using ComponentMultimap = std::unordered_multimap<std::type_index, ComponentID>;

class GObject {

public:
    GObject(GObjectID id) : m_id(id) {}
    ~GObject() = default;

    void initialize(const std::string& path);

    void tick(float delta_time);

    GObjectID getID() const { return m_id; }

    /**
     * @brief Get the id of the parent object
     */
    GObjectID getParentObjectID() const { return m_parent_object; }

    /**
     * @brief Whether have the specific object
     */
    bool hasObject(const std::string& name) const { return m_child_objects.find(name) != m_child_objects.end(); }

    /**
     * @brief Get the id of the specific object
     */
    GObjectID getObjectID(const std::string& name) const { return m_child_objects.at(name); }

    /**
     * @brief Get the ids of all objects
     */
    const std::unordered_map<std::string, GObjectID>& getObjectsID() const { return m_child_objects; }

    /**
     * @brief Add a object
     */
    void addObject(std::shared_ptr<GObject> object);

    /**
     * @brief Get the number of components that match component_type
     * @param component_type The type of component needed to be filtered
     *                       Usage: "typeid(TransformComponent)" will get the type of transform component
     */
    int hasComponent(const std::type_index& component_type) const { return m_components.count(component_type); }

    /**
     * @brief Get all of components in this GObject
     */
    const std::unordered_multimap<std::type_index, ComponentID>& getComponentsID() const { return m_components; }

    /**
     * @brief Get the specific type components in this GObject
     * @return A pair of iterators. Usage: for(auto it = return_value.first; it != return_value.second; it++) {}
     */
    std::pair<ComponentMultimap::const_iterator, ComponentMultimap::const_iterator>
    getComponentsID(const std::type_index& component_type) const {
        return m_components.equal_range(component_type);
    }

    /**
     * @brief Add a component
     */
    void addComponent(std::shared_ptr<Component> component);

private:
    GObjectID                                  m_id;
    GObjectID                                  m_parent_object;
    std::unordered_map<std::string, GObjectID> m_child_objects;

    // key is "typeid" of a component class, e.g. typeid(TransformComponent)
    // value is a ComponentID
    std::unordered_multimap<std::type_index, ComponentID> m_components;
};

} // namespace BJTUGE