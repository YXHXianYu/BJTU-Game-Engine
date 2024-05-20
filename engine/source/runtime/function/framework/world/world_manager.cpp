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
            auto add_block = [&](int x, int y, int z, BlockId id) {
                std::dynamic_pointer_cast<BlockManagerComponent>(m_components[0])->add_block(x, y, z, id);
            };
            auto remove_block = [&](int x, int y, int z) {
                std::dynamic_pointer_cast<BlockManagerComponent>(m_components[0])->remove_block(x, y, z);
            };

            int len = 40;
            for (int i = -len; i <= len; i++) {
                for (int j = -len; j <= len; j++) {
                    for (int h = -10; h <= -1; h++) {
                        BlockId id = (h == -1) ? (BlockId::grass) : (h >= -4) ? (BlockId::dirt) : (BlockId::stone);
                        add_block(i, h, j, id);
                    }
                }
            }
            {
                int x_min = -15;
                int x_max = 15;
                int y_min = 2;
                int y_max = 15;
                for (int i = x_min; i <= x_max; i++) {
                    for (int j = y_min; j <= y_max; j++) {
                        for (int h = (i == x_min || i == x_max || j == y_min || j == y_max) ? -1 : -2; h <= -1; h++) {
                            remove_block(i, h, j);
                        }
                    }
                }
            }

            // building
            int sx = -10; // start x
            int sz = -10; // start z
            for (int y = 0; y <= 3; y++) {
                add_block(sx, y, sz, BlockId::oak_log);
                add_block(sx, y, sz + 5, BlockId::oak_log);
                add_block(sx + 5, y, sz, BlockId::oak_log);
                add_block(sx + 5, y, sz + 5, BlockId::oak_log);

                for (int x = sx + 1; x <= sx + 4; x++)
                    add_block(x, y, sz, BlockId::oak_planks);
                for (int x = sx + 1; x <= sx + 4; x++)
                    add_block(x, y, sz + 5, BlockId::oak_planks);
                for (int z = sz + 1; z <= sz + 4; z++)
                    add_block(sx, y, z, BlockId::oak_planks);
                for (int z = sz + 1; z <= sz + 4; z++)
                    add_block(sx + 5, y, z, BlockId::oak_planks);
            }
            for (int x = sx + 1; x <= sx + 4; x++)
                for (int z = sz + 1; z <= sz + 4; z++)
                    add_block(x, 3, z, BlockId::oak_planks);

            // door
            remove_block(sx + 2, 0, sz + 5);
            remove_block(sx + 2, 1, sz + 5);
            remove_block(sx + 3, 0, sz + 5);
            remove_block(sx + 3, 1, sz + 5);

            // test
            for (uint32_t i = 0; i < static_cast<uint32_t>(BlockId::block_id_count); i++) {
                add_block(5, 0, -10 + i, BlockId(i));
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
