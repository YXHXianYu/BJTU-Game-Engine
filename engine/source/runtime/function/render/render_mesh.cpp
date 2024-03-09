#include "runtime/function/render/render_mesh.h"

#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_texture.h"
#include "runtime/function/render/render_shader.h"

#include <iostream>

namespace BJTUGE {

RenderMesh::RenderMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<std::string>& textures)
    : m_vertices(vertices), m_indices(indices), m_textures(textures) {
    // 0. generate vao, vbo, ebo
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    if (m_indices.size() > 0) { glGenBuffers(1, &m_ebo); }
    // 1. bind vao
    glBindVertexArray(m_vao);
    // 2. copy vertex data to gpu
    // 2.1 vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0].position.x, GL_STATIC_DRAW);
    if (m_ebo > 0) {
        // 2.2 ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), &m_indices[0], GL_STATIC_DRAW);
    }
    // 3. set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

RenderMesh::~RenderMesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    if (m_ebo > 0) { glDeleteBuffers(1, &m_ebo); }
}

void RenderMesh::draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) {
    shader->setUniform("u_model", model * m_model);
    for (auto i = 0; i < m_textures.size(); i++) {
        auto texture = resource->getTexture(m_textures[i]);
        assert(texture);
        texture->use(shader, "u_texture_" + texture->getType(), i);
    }
    if (m_textures.size() > 1) {
        std::cout << "(" << m_textures.size() << "): ";
        for (auto id : m_textures) {
            std::cout << resource->getTexture(id)->getType() << " ";
        }
        std::cout << std::endl;
    }

    use();
    if (m_ebo > 0) {
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }
}

} // namespace BJTUGE
