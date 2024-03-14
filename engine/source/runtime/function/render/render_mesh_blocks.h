#pragma once

#include "runtime/function/render/render_mesh_base.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace BJTUGE {

struct FaceInfo {
    glm::vec3 position;
    float     face;
    float     material_id; // float储存 [0, 2^23-1] 的整数是精确的，不会发生精度损失

    FaceInfo() : position(glm::vec3{0.0f}), face(0.0f), material_id(-1.0f) {} // default to create a invalid face
    FaceInfo(float x, float y, float z, float f, float id) : position(glm::vec3{x, y, z}), face(f), material_id(id) {}
};

class RenderMeshBlocks : public RenderMeshBase {

public:
    RenderMeshBlocks(const std::vector<FaceInfo>& blocks);
    virtual ~RenderMeshBlocks() override;
    RenderMeshBlocks(const RenderMeshBlocks&)            = delete;
    RenderMeshBlocks& operator=(const RenderMeshBlocks&) = delete;

    virtual void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) override;

    void mapBuffer();
    void unmapBuffer();
    void updateBuffer(uint32_t pos, FaceInfo data);
    bool isMapped() const { return m_ptr != nullptr; }

private:
    void setModelMatrix(const glm::mat4& model) { m_model = model; } // private to modify model matrix of mesh blocks

private:
    static const std::vector<float>  m_vertices;
    static const std::vector<Vertex> m_cube;

    std::vector<FaceInfo> m_blocks;
    glm::mat4             m_model{1.0f};

    uint32_t m_vao{0};
    uint32_t m_vbo_blocks{0};
    uint32_t m_vbo_vertices{0};

    void* m_ptr{nullptr};

private:
    std::unordered_set<void*> m_shader_tag; // 用于给一些shader添加tag，避免在draw时重复设置uniform
};

} // namespace BJTUGE
