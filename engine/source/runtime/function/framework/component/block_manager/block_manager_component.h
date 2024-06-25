#pragma once

#include "runtime/function/framework/component/component.h"

#include <array>
#include <unordered_map>
#include <vector>

namespace std {

template <> struct hash<std::array<int32_t, 3>> {
    size_t operator()(const std::array<int32_t, 3>& key) const {
        return std::hash<int32_t>()(key[0]) ^ std::hash<int32_t>()(key[1]) ^ std::hash<int32_t>()(key[2]);
    }
};

} // namespace std

namespace BJTUGE {

enum class BlockId : uint32_t {
    air = 0,
    stone,
    grass,
    dirt,
    cobblestone,
    oak_planks,
    oak_log,
    oak_leaves,
    red_wool,
    red_wool_brighter,
    water,
    block_id_count,
};

enum class TransparentType : uint32_t {
    none = 0,
    opaque,
    water_transparent,
    leaves_transparent,
};

struct BlockProperty {
    TransparentType transparent_type;
};

class BlockManagerComponent : public Component {

public:
    void initialize(std::weak_ptr<GObject> parent_object) override;
    void tick(float delta_time) override;

    void add_block(int32_t x, int32_t y, int32_t z, BlockId block_id);
    void remove_block(int32_t x, int32_t y, int32_t z);

private:
    void update_block(int32_t x, int32_t y, int32_t z, uint32_t block_id, bool is_add);
    void update_block_face(int32_t x, int32_t y, int32_t z, uint32_t block_id, bool is_add);

    std::unordered_map<std::array<int32_t, 3>, BlockId> m_blocks;

    const std::array<std::array<int32_t, 3>, 6> m_directions = {std::array<int32_t, 3>{0, 1, 0}, std::array<int32_t, 3>{0, -1, 0},
                                                                std::array<int32_t, 3>{0, 0, 1}, std::array<int32_t, 3>{0, 0, -1},
                                                                std::array<int32_t, 3>{1, 0, 0}, std::array<int32_t, 3>{-1, 0, 0}};

    const std::vector<BlockProperty> m_block_properties = {
        BlockProperty{TransparentType::none},               // air
        BlockProperty{TransparentType::opaque},             // stone
        BlockProperty{TransparentType::opaque},             // grass
        BlockProperty{TransparentType::opaque},             // dirt
        BlockProperty{TransparentType::opaque},             // cobblestone
        BlockProperty{TransparentType::opaque},             // oak_planks
        BlockProperty{TransparentType::opaque},             // oak_log
        BlockProperty{TransparentType::leaves_transparent}, // oak_leaves
        BlockProperty{TransparentType::opaque},             // red_wool
        BlockProperty{TransparentType::opaque},             // red_wool_brighter
        BlockProperty{TransparentType::water_transparent},  // water
    };
};

} // namespace BJTUGE