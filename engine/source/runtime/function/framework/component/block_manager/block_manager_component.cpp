#include "runtime/function/framework/component/block_manager/block_manager_component.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/swap/minecraft_blocks/add_block_swap_event.h"
#include "runtime/function/swap/minecraft_blocks/remove_block_swap_event.h"
#include "runtime/function/swap/swap_context.h"

namespace BJTUGE {

void BlockManagerComponent::initialize(std::weak_ptr<GObject> parent_object) {
    m_parent_object = parent_object;
}

void BlockManagerComponent::tick(float delta_time) {}

void BlockManagerComponent::add_block(int32_t x, int32_t y, int32_t z, BlockId block_id) {
    std::array<int32_t, 3> key = {x, y, z};
    assert(m_blocks.find(key) == m_blocks.end());

    uint32_t id   = static_cast<uint32_t>(block_id);
    m_blocks[key] = block_id;

    update_block(x, y, z, id, true);
}

void BlockManagerComponent::remove_block(int32_t x, int32_t y, int32_t z) {
    std::array<int32_t, 3> key = {x, y, z};
    assert(m_blocks.find(key) != m_blocks.end());

    uint32_t id = static_cast<uint32_t>(m_blocks.at(key));
    m_blocks.erase(key);

    update_block(x, y, z, id, false);
}

void BlockManagerComponent::update_block(int32_t x, int32_t y, int32_t z, uint32_t id, bool is_add) {
    auto func = [&](int32_t x, int32_t y, int32_t z, uint32_t face, bool is_add) {
        if (is_add)
            g_runtime_global_context.m_swap_context->push(std::make_unique<AddBlockSwapEvent>(FaceInfo{x, y, z, face, id}));
        else
            g_runtime_global_context.m_swap_context->push(std::make_unique<RemoveBlockSwapEvent>(glm::vec3(x, y, z), face));
    };

    for (uint32_t face = 0; face < 6; face++) {
        func(x, y, z, face, is_add);
    }
}

void BlockManagerComponent::update_block_face(int32_t x, int32_t y, int32_t z, uint32_t id, bool is_add) {
    TransparentType transparent_type = m_block_properties[id].transparent_type;

    auto func = [&](int32_t x, int32_t y, int32_t z, uint32_t face, bool is_add) {
        if (is_add)
            g_runtime_global_context.m_swap_context->push(std::make_unique<AddBlockSwapEvent>(FaceInfo{x, y, z, face, id}));
        else
            g_runtime_global_context.m_swap_context->push(std::make_unique<RemoveBlockSwapEvent>(glm::vec3(x, y, z), face));
    };

    if (transparent_type == TransparentType::none) return; // Air Block

    if (transparent_type == TransparentType::leaves_transparent) {
        for (uint32_t face = 0; face < 6; face++) {
            func(x, y, z, face, is_add);
        }
    } else if (transparent_type == TransparentType::water_transparent) {
        for (uint32_t face = 0; face < 6; face++) {
            auto dx = x + m_directions[face][0];
            auto dy = y + m_directions[face][1];
            auto dz = z + m_directions[face][2];

            auto it = m_blocks.find({dx, dy, dz});
            if (it != m_blocks.end() && id == static_cast<uint32_t>(it->second)) {
                func(x, y, z, face ^ 1, is_add ^ 1);
            } else {
                func(x, y, z, face, is_add);
            }
        }
    } else if (transparent_type == TransparentType::opaque) {
        for (uint32_t face = 0; face < 6; face++) {
            auto dx = x + m_directions[face][0];
            auto dy = y + m_directions[face][1];
            auto dz = z + m_directions[face][2];

            auto it = m_blocks.find({dx, dy, dz});
            if (it != m_blocks.end() && transparent_type == m_block_properties[static_cast<uint32_t>(it->second)].transparent_type) {
                func(x, y, z, face ^ 1, is_add ^ 1);
            } else {
                func(x, y, z, face, is_add);
            }
        }
    } else {
        assert(false);
    }
}

} // namespace BJTUGE