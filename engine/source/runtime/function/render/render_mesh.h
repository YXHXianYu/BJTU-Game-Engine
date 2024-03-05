#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace BJTUGE {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

class RenderMesh {

public:
    RenderMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    uint32_t getVAO() const { return m_vao; }

private:
    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;

    uint32_t m_vao{0};
    uint32_t m_vbo{0};
    uint32_t m_ebo{0};
};

} // namespace BJTUGE
