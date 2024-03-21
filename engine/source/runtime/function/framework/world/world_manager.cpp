#include "runtime/function/framework/world/world_manager.h"

#include "runtime/function/framework/level/level.h"

// tmp (should be removed in the future)
#include "runtime/function/global/global_context.h"
#include "runtime/function/swap/minecraft_blocks/add_block_swap_event.h"
#include "runtime/function/swap/minecraft_blocks/remove_block_swap_event.h"
#include "runtime/function/swap/swap_context.h"
#include <iostream>

#include "runtime/function/framework/component/block_manager/block_manager_component.h"

namespace BJTUGE {

void WorldManager::initialize() {
    m_levels["overworld"] = std::make_shared<Level>();
    m_levels["overworld"]->initialize();

    m_components[0] = std::make_shared<BlockManagerComponent>();
}

void WorldManager::tick(float delta_time) {
    for (auto& [key, level] : m_levels) {
        level->tick(delta_time);
    }

    {
        static uint32_t tick_count = 0;
        tick_count += 1;

        if (tick_count == 1) {
            int len  = 100;
            int len2 = 5;
            for (int i = -len; i <= len; i++) {
                for (int j = -len; j <= len; j++) {
                    for (int k = 0; k < len2; k++) {
                        int     h  = ((i + j + k) % 10 + 10) % 10 - 5;
                        BlockId id = (h == 4) ? (BlockId::grass) : (h >= 0) ? (BlockId::dirt) : (BlockId::stone);
                        std::dynamic_pointer_cast<BlockManagerComponent>(m_components[0])->add_block(i, h, j, id);
                    }
                }
            }
        }

        if (tick_count == 1) {
            int len = 3;
            for (int i = -len; i <= len; i++) {
                for (int j = -len; j <= len; j++) {
                    for (int h = 20; h <= 21; h++) {
                        BlockId id = BlockId::grass;
                        std::dynamic_pointer_cast<BlockManagerComponent>(m_components[0])->add_block(i, h, j, id);
                    }
                }
            }
        }
    }

    // Test
    if (false) {
        static uint32_t tick_count = 0;
        tick_count += 1;

        if (tick_count == 1) {
            int len = 100;
            for (int i = -len; i <= len; i++) {
                for (int j = -len; j <= len; j++) {
                    g_runtime_global_context.m_swap_context->push(
                        std::make_unique<AddBlockSwapEvent>(FaceInfo{float(i), 0.f, float(j), 0.f, float((i + j + 1000) % 6)}));
                }
            }
        }

        if (tick_count % 1000 == 0) { std::cout << "tick_count: " << tick_count << std::endl; }

        if (1000 <= tick_count && tick_count <= 2000) {
            int len  = 51;
            int len2 = 50;
            for (int i = -len; i <= len; i++) {
                for (int j = -len; j <= len; j++) {
                    if (-len2 <= i && i <= len2 && -len2 <= j && j <= len2) continue;

                    if (tick_count % 2 == 0)
                        g_runtime_global_context.m_swap_context->push(
                            std::make_unique<RemoveBlockSwapEvent>(glm::vec3(float(i), 0.f, float(j)), 0.f));
                    else
                        g_runtime_global_context.m_swap_context->push(
                            std::make_unique<AddBlockSwapEvent>(FaceInfo{float(i), 0.f, float(j), 0.f, float((i + j + 100) % 6)}));
                }
            }
        }
    }
}

void WorldManager::clear() {}

} // namespace BJTUGE