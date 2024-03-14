#pragma once

#include "runtime/core/hash_func.h"

#include <glm/fwd.hpp>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace BJTUGE {

class RenderEntity;
class RenderMeshBlocks;
class GMemoryBuffer;
class GMemoryBufferIndex;
class FaceInfo;

class RenderMinecraftBlocksManager {

public:
    void initialize();
    void clear();

    void startTransfer();
    void endTransfer();
    void insert(const FaceInfo& face);
    void remove(glm::vec3 position);

    std::shared_ptr<RenderEntity> getEntity() { return m_entity; }

public:
    static const uint32_t k_buffer_size; // represents the number of faces that a buffer can store

private:
    void                              addBuffer();
    std::shared_ptr<RenderMeshBlocks> createMeshBlocks();

private:
    std::vector<GMemoryBuffer>                        m_buffers;
    std::vector<std::shared_ptr<RenderMeshBlocks>>    m_meshes;
    std::unordered_map<glm::vec3, GMemoryBufferIndex> m_index;

    std::shared_ptr<RenderEntity> m_entity;
    bool                          m_is_transferring;
};

} // namespace BJTUGE