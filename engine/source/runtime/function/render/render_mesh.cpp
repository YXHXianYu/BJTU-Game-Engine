#include "runtime/function/render/render_mesh.h"

#include <iostream>

namespace BJTUGE {

RenderMesh::RenderMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : m_vertices(vertices), m_indices(indices) {
    // 0. generate vao, vbo, ebo
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    // 1. bind vao
    glBindVertexArray(m_vao);
    // 2. copy vertex data to gpu
    // 2.1 vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0].position.x, GL_STATIC_DRAW);
    // 2.2 ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), &m_indices[0], GL_STATIC_DRAW);
    // 3. set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

} // namespace BJTUGE
