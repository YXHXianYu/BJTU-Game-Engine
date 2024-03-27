#include "runtime/function/framework/level/level.h"

#include "runtime/function/framework/object/object.h"
#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"

#include <cassert>

namespace BJTUGE {

void Level::initialize() {}

void Level::tick(float delta_time) {
    for (auto& [name, id] : m_objects) {
        g_runtime_global_context.m_world_manager->getObject(id)->tick(delta_time);
    }
}

} // namespace BJTUGE