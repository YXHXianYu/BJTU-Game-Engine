#include "runtime/function/framework/world/world_manager.h"

#include "runtime/function/framework/level/level.h"

namespace BJTUGE {

void WorldManager::initialize() {}

void WorldManager::tick(float delta_time) {
    for (auto& [key, level] : m_levels) {
        level->tick(delta_time);
    }
}

void WorldManager::clear() {}

} // namespace BJTUGE