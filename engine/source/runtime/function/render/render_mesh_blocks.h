#pragma once

#include "runtime/function/render/render_mesh_base.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace BJTUGE {

struct BlockInfo {
    glm::vec3 position;
    float     face;
    float     material_id; // float储存 [0, 2^23-1] 的整数是精确的，不会发生精度损失

    BlockInfo() : position(glm::vec3{0.0f}), face(0.0f), material_id(0.0f) {}
    BlockInfo(float x, float y, float z, float f, float id) : position(glm::vec3{x, y, z}), face(f), material_id(id) {}
};

class RenderMeshBlocks : public RenderMeshBase {

public:
    RenderMeshBlocks(std::vector<BlockInfo> blocks);
    virtual ~RenderMeshBlocks() override;
    RenderMeshBlocks(const RenderMeshBlocks&)            = delete;
    RenderMeshBlocks& operator=(const RenderMeshBlocks&) = delete;

    virtual void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) override;

private:
    void setModelMatrix(const glm::mat4& model) { m_model = model; } // private to modify model matrix of mesh blocks

private:
    static const std::vector<float>  m_vertices;
    static const std::vector<Vertex> m_cube;

    std::vector<BlockInfo> m_blocks;
    glm::mat4              m_model{1.0f};

    uint32_t m_vao{0};
    uint32_t m_vbo_blocks{0};
    uint32_t m_vbo_vertices{0};

private:
    std::unordered_set<void*> m_shader_tag; // 用于给一些shader添加tag，避免在draw时重复设置uniform
};

} // namespace BJTUGE
