#pragma once

#include "runtime/function/framework/framework_common.h"

#include <cstddef>
#include <memory>

namespace BJTUGE {

class Component {

public:
    Component()          = default;
    virtual ~Component() = 0;

    virtual void initialize(std::weak_ptr<GObject> parent_object) { m_parent_object = parent_object; }

    virtual void tick(float delta_time) = 0;

    bool isDirty() const { return m_is_dirty; }
    void setIsDirty(bool is_dirty) { m_is_dirty = is_dirty; }

    bool isActive() const { return m_is_active; }
    void setIsActive(bool is_active) { m_is_active = is_active; }

protected:
    std::weak_ptr<GObject> m_parent_object;
    bool                   m_is_dirty{false};
    bool                   m_is_active{true};
};

} // namespace BJTUGE