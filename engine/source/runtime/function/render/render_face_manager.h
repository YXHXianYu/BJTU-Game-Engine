#pragma once

#include "runtime/function/render/render_mesh_blocks.h"

#include <bitset>
#include <cstdint>
#include <queue>

namespace BJTUGE {

struct FaceInfo;

class GMemoryBuffer {
    std::vector<bool>    m_flags;
    std::queue<uint32_t> m_free_indices;
    uint32_t             m_size;
    uint32_t             m_left;

    // TODO: constructor
    // TODO: allocator
    // TODO: get left space
};

class RenderFaceManager {

public:
    void initialize();
    void clear();

    void enableCache();
    void disableCache();
    void updateCache();
    void setCacheSize(uint32_t size);

public:
    static const uint32_t k_buffer_size; // represents the number of faces that a buffer can store
    static const uint32_t k_cache_size;  // represents the number of faces that a cache can store
    // TODO: multi buffer...

private:
};

} // namespace BJTUGE