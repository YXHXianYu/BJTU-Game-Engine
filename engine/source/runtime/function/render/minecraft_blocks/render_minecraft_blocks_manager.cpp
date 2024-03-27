#include "runtime/function/render/minecraft_blocks/render_minecraft_blocks_manager.h"

#include "runtime/function/render/face_info.h"
#include "runtime/function/render/minecraft_blocks/gmemory_buffer.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_mesh_blocks.h"

#include <glm/glm.hpp>

#include <iostream>

namespace BJTUGE {

const uint32_t RenderMinecraftBlocksManager::k_buffer_size = 1024 * 1024;
// const uint32_t RenderMinecraftBlocksManager::k_buffer_size = 1024;

void RenderMinecraftBlocksManager::initialize() {
    m_entity = std::make_shared<RenderEntity>();

    addBuffer(false);

    // ===== For Test =====
    startTransfer();
    for (int i = 0; i < 9; i++) {
        insert(FaceInfo{0.0f, 10.0f, i * 1.0f, 0.f, (float)(i % 9)});
    }
    endTransfer();
}

void RenderMinecraftBlocksManager::clear() {
    m_entity.reset();
}

void RenderMinecraftBlocksManager::startTransfer() {
    m_is_transferring = true;
    for (uint32_t i = 0; i < m_buffers.size(); i++) {
        m_meshes[i]->mapBuffer();
    }
}

void RenderMinecraftBlocksManager::endTransfer() {
    m_is_transferring = false;
    for (uint32_t i = 0; i < m_buffers.size(); i++) {
        m_meshes[i]->unmapBuffer();
    }
}

void RenderMinecraftBlocksManager::insert(const FaceInfo& face) {
    assert(m_is_transferring);
    for (uint32_t i = 0; i < m_buffers.size(); i++) {
        uint32_t index = m_buffers[i].allocate(); // allocate logical buffer
        if (index != -1) {
            m_meshes[i]->updateBuffer(index, face);                             // allocate physical buffer
            m_index[{face.position, face.face}] = GMemoryBufferIndex{i, index}; // update index
            return;
        }
    }

    addBuffer(true);                                                                                 // create a new buffer
    uint32_t index = m_buffers.back().allocate();                                                    // allocate logical buffer
    m_meshes.back()->updateBuffer(index, face);                                                      // allocate physical buffer
    m_index[{face.position, face.face}] = GMemoryBufferIndex{uint32_t(m_buffers.size() - 1), index}; // update index
}

void RenderMinecraftBlocksManager::remove(glm::vec3 position, float face) {
    assert(m_is_transferring);
    auto iter = m_index.find({position, face}); // find index by position
    assert(iter != m_index.end());
    uint32_t index  = iter->second.index;
    uint32_t offset = iter->second.offset;
    m_index.erase(iter);                               // update index
    m_meshes[index]->updateBuffer(offset, FaceInfo()); // release physical buffer
    m_buffers[index].deallocate(offset);               // release logical buffer
}

void RenderMinecraftBlocksManager::addBuffer(bool enable_map) {
    m_buffers.push_back(GMemoryBuffer(k_buffer_size)); // create a new logical buffer
    m_meshes.push_back(createMeshBlocks());            // create a new physical buffer
    if (enable_map) m_meshes.back()->mapBuffer();

    m_entity->addMesh(std::to_string(m_meshes.size()), m_meshes.back());
}

std::shared_ptr<RenderMeshBlocks> RenderMinecraftBlocksManager::createMeshBlocks() {
    std::vector<FaceInfo>             blocks(k_buffer_size);
    std::shared_ptr<RenderMeshBlocks> mesh = std::make_shared<RenderMeshBlocks>(blocks);
    return mesh;
}

} // namespace BJTUGE