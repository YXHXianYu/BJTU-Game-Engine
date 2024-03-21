#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

namespace BJTUGE {

class RenderMeshBlocks;

class GMemoryBuffer {
public:
    GMemoryBuffer(uint32_t size);
    uint32_t allocate();
    void     deallocate(uint32_t index);
    int      left();

private:
    std::vector<bool>    m_flags;
    std::queue<uint32_t> m_free_indices;
    uint32_t             m_size;
    uint32_t             m_left;
};

struct GMemoryBufferIndex {
    uint32_t index;
    uint32_t offset;

    GMemoryBufferIndex() : index(0), offset(0) {}
    GMemoryBufferIndex(uint32_t index, uint32_t offset) : index(index), offset(offset) {}
};

} // namespace BJTUGE
