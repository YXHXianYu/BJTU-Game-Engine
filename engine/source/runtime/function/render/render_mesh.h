#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

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

class RenderMesh {

public:
    RenderMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    uint32_t getVAO() const { return m_vao; }
    void     use() const { glBindVertexArray(m_vao); }
    void     bind() const { use(); }

private:
    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;

    uint32_t m_vao{0};
    uint32_t m_vbo{0};
    uint32_t m_ebo{0};
};

} // namespace BJTUGE
