#include "runtime/function/render/render_mesh.h"

#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_texture.h"

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
    // 2.2 ebo
    if (m_ebo > 0) {
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

void RenderMesh::addTexture(const std::string& path, std::shared_ptr<RenderTexture> texture, std::shared_ptr<RenderResource> resource) {
    resource->registerTexture(path, texture);
    m_textures.push_back(path);
}

void RenderMesh::draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) {
    model = model * m_model;
    shader->setUniform("u_model", model);
    shader->setUniform("u_normal", glm::transpose(glm::inverse(model)));
    shader->setUniform("u_texture_cnt", static_cast<int>(m_textures.size()));
    shader->setUniform("u_diffuse_color", m_diffuse_color);
    shader->setUniform("u_texture_diffuse", 0);
    for (auto i = 0; i < m_textures.size(); i++) {
        auto texture = std::dynamic_pointer_cast<RenderTexture>(resource->getTexture(m_textures[i]));
        assert(texture);
        texture->use(shader, "u_texture_" + texture->getType(), i);
    }
    // assert(m_textures.size() == 1); // TODO: what is this for?
    // Answer: 为了避免在shader中使用了多个纹理，但是没有设置uniform texture...的情况

    glBindVertexArray(m_vao);
    if (m_ebo > 0) {
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }
}

void RenderMesh::output() const {
    std::cout << "{" << std::endl;
    for (const auto& v : m_vertices) {
        std::cout << "    Vertex{";
        std::cout << v.position.x << ", " << v.position.y << ", " << v.position.z << ", ";
        std::cout << v.normal.x << ", " << v.normal.y << ", " << v.position.z << ", ";
        std::cout << v.texcoord.x << ", " << v.texcoord.y << "}," << std::endl;
    }
    std::cout << "}" << std::endl;
    std::cout << "{" << std::endl;
    for (int i = 0; i < m_indices.size(); i += 3) {
        std::cout << "    " << m_indices[i] << ", " << m_indices[i + 1] << ", " << m_indices[i + 2] << std::endl;
    }
    std::cout << "}" << std::endl;
}

} // namespace BJTUGE
