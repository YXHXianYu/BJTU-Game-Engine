#include "runtime/function/render/minecraft_blocks/gmemory_buffer.h"

namespace BJTUGE {

GMemoryBuffer::GMemoryBuffer(uint32_t size) : m_size(size), m_left(size) {
    m_flags.resize(size, 0);
    for (uint32_t i = 0; i < size; i++) {
        m_free_indices.push(i);
    }
}

uint32_t GMemoryBuffer::allocate() {
    if (m_free_indices.empty()) { return -1; }
    uint32_t index = m_free_indices.front();
    m_free_indices.pop();
    m_flags[index] = 1;
    m_left--;
    return index;
}

void GMemoryBuffer::deallocate(uint32_t index) {
    if (index >= m_size) { return; }
    if (m_flags[index] == 0) { return; }
    m_flags[index] = 0;
    m_free_indices.push(index);
    m_left++;
}

int GMemoryBuffer::left() {
    return m_left;
}

} // namespace BJTUGE