#pragma once

#include "runtime/function/framework/framework_common.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

class GObject;

class Level {

public:
    Level()  = default;
    ~Level() = default;

    void initialize();
    void tick(float delta_time);

    void      addObject(const std::string& name, GObjectID id) { m_objects[name] = id; }
    void      removeObject(const std::string& name) { m_objects.erase(name); }
    bool      hasObject(const std::string& name) const { return m_objects.find(name) != m_objects.end(); }
    GObjectID getObject(const std::string& name) const {
        assert(hasObject(name));
        return m_objects.at(name);
    }

private:
    std::unordered_map<std::string, GObjectID> m_objects;
};

} // namespace BJTUGE