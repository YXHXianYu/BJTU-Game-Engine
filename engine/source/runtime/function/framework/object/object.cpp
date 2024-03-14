#include "runtime/function/framework/object/object.h"

#include "runtime/function/framework/component/component.h"
#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"

namespace BJTUGE {

void GObject::initialize(const std::string& path) {}

void GObject::tick(float delta_time) {
    for (auto& [type, id] : m_components) {
        g_runtime_global_context.m_world_manager->getComponent(id)->tick(delta_time);
    }
}

void GObject::addObject(std::shared_ptr<GObject> object) {}

void GObject::addComponent(std::shared_ptr<Component> component) {}

} // namespace BJTUGE