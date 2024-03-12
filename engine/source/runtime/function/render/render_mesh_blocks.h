#pragma once

#include "runtime/function/render/render_mesh_base.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

struct BlockInfo {
    glm::vec3 position;
    float     face;
    float     material_id; // float储存 [0, 2^23-1] 的整数是精确的，不会发生精度损失
};

// todo

class RenderMeshBlocks : public RenderMeshBase {

public:
    RenderMeshBlocks(std::vector<BlockInfo> blocks);
    virtual ~RenderMeshBlocks() override;
    RenderMeshBlocks(const RenderMeshBlocks&)            = delete;
    RenderMeshBlocks& operator=(const RenderMeshBlocks&) = delete;

    virtual void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) override;

    void setModelMatrix(const glm::mat4& model) { m_model = model; }

private:
    static const std::vector<float>    m_vertices;
    static const std::vector<uint32_t> m_indices;

    std::vector<BlockInfo> m_blocks;
    glm::mat3              m_model{1.0f};

    uint32_t m_vao{0};
    uint32_t m_vbo{0};
    uint32_t m_ebo{0};
};

} // namespace BJTUGE