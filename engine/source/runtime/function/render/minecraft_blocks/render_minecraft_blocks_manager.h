#pragma once

#include "runtime/core/hash_func.h"
#include "runtime/function/render/minecraft_blocks/gmemory_buffer.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace std {

template <> struct hash<std::pair<glm::vec3, float>> {
    size_t operator()(const std::pair<glm::vec3, float>& v) const {
        std::hash<float> hasher;
        auto             hashX = hasher(v.first.x);
        auto             hashY = hasher(v.first.y);
        auto             hashZ = hasher(v.first.z);
        auto             hashW = hasher(v.second);

        return hashX ^ (hashY << 1) ^ (hashZ << 2) ^ (hashW << 3);
    }
};

} // namespace std

namespace BJTUGE {

class RenderEntity;
class RenderMeshBlocks;
class GMemoryBuffer;
class GMemoryBufferIndex;
struct FaceInfo;

class RenderMinecraftBlocksManager {

public:
    void initialize();
    void clear();

    void startTransfer();
    void endTransfer();
    void insert(const FaceInfo& face);
    void remove(glm::vec3 position, float face);

    std::shared_ptr<RenderEntity> getEntity() { return m_entity; }

public:
    static const uint32_t k_buffer_size; // represents the number of faces that a buffer can store

private:
    void                              addBuffer(bool enable_map);
    std::shared_ptr<RenderMeshBlocks> createMeshBlocks();

private:
    std::vector<GMemoryBuffer>                                          m_buffers;
    std::vector<std::shared_ptr<RenderMeshBlocks>>                      m_meshes;
    std::unordered_map<std::pair<glm::vec3, float>, GMemoryBufferIndex> m_index;

    // TODO: index from vec3 to vec3+face

    // This entity will be binded to RenderResource, and every modification of the entity will be synchronized to the RenderResource
    std::shared_ptr<RenderEntity> m_entity;

    bool m_is_transferring;
};

} // namespace BJTUGE
