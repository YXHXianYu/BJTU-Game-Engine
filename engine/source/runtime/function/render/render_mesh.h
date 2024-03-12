#pragma once

#include "runtime/function/render/render_mesh_base.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;

    Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texcoord)
        : position(position), normal(normal), texcoord(texcoord) {}
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
        : position(x, y, z), normal(nx, ny, nz), texcoord(u, v) {}
};

class RenderMesh : public RenderMeshBase {

public:
    RenderMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<std::string>& textures = {});
    virtual ~RenderMesh() override;
    RenderMesh(const RenderMesh&)            = delete;
    RenderMesh& operator=(const RenderMesh&) = delete;

    void addTexture(const std::string& texture) { m_textures.push_back(texture); }

    virtual void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) override;

    void setModelMatrix(const glm::mat4& model) { m_model = model; }

private:
    std::vector<Vertex>      m_vertices;
    std::vector<uint32_t>    m_indices;
    std::vector<std::string> m_textures;
    glm::mat4                m_model{1.0f};

    uint32_t m_vao{0};
    uint32_t m_vbo{0};
    uint32_t m_ebo{0};
};

} // namespace BJTUGE
