#pragma once

#include "runtime/function/framework/framework_common.h"

#include <cassert>
#include <memory>
#include <unordered_map>

namespace BJTUGE {

class WorldManager {

public:
    WorldManager()  = default;
    ~WorldManager() = default;

    void initialize();
    void tick(float delta_time);
    void clear();

    void                     addObject(GObjectID id, std::shared_ptr<GObject> object) { m_objects[id] = object; }
    void                     removeObject(GObjectID id) { m_objects.erase(id); }
    bool                     hasObject(GObjectID id) const { return m_objects.find(id) != m_objects.end(); }
    std::shared_ptr<GObject> getObject(GObjectID id) const {
        assert(hasObject);
        return m_objects.at(id);
    }

    void                       addComponent(ComponentID id, std::shared_ptr<Component> component) { m_components[id] = component; }
    void                       removeComponent(ComponentID id) { m_components.erase(id); }
    bool                       hasComponent(ComponentID id) const { return m_components.find(id) != m_components.end(); }
    std::shared_ptr<Component> getComponent(ComponentID id) const {
        assert(hasComponent);
        return m_components.at(id);
    }

    void                   addLevel(const std::string& name, std::shared_ptr<Level> level) { m_levels[name] = level; }
    void                   removeLevel(const std::string& name) { m_levels.erase(name); }
    bool                   hasLevel(const std::string& name) const { return m_levels.find(name) != m_levels.end(); }
    std::shared_ptr<Level> getLevel(const std::string& name) const {
        assert(hasLevel);
        return m_levels.at(name);
    }

private:
    std::unordered_map<GObjectID, std::shared_ptr<GObject>>     m_objects;
    std::unordered_map<ComponentID, std::shared_ptr<Component>> m_components;

    std::unordered_map<std::string, std::shared_ptr<Level>> m_levels;
};

} // namespace BJTUGE