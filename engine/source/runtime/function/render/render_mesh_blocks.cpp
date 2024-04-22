#include "runtime/function/render/render_mesh_blocks.h"

#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_texture_base.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstring>

namespace BJTUGE {

const std::vector<float> RenderMeshBlocks::m_vertices = {0.0f, 1.0f, 2.0f, 3.0f};

// here 0.49999 is to avoid z-fighting
const std::vector<Vertex> RenderMeshBlocks::m_cube = {
    Vertex{0.5, 0.49999, -0.5, 0, 1, 0, 0.625, 0.5},      Vertex{-0.5, 0.49999, -0.5, 0, 1, 0, 0.875, 0.5},
    Vertex{-0.5, 0.49999, 0.5, 0, 1, 0, 0.875, 0.75},      Vertex{0.5, 0.49999, 0.5, 0, 1, 0, 0.625, 0.75},

    Vertex{-0.5, -0.49999, -0.5, 0, -1, 0, 0.125, 0.5},   Vertex{0.5, -0.49999, -0.5, 0, -1, 0, 0.375, 0.5},
    Vertex{0.5, -0.49999, 0.5, 0, -1, 0, 0.375, 0.75},     Vertex{-0.5, -0.49999, 0.5, 0, -1, 0, 0.125, 0.75},

    Vertex{0.5, -0.5, 0.49999, 0, 0, 1, 0.375, 0.75},     Vertex{0.5, 0.5, 0.49999, 0, 0, 1, 0.625, 0.75},
    Vertex{-0.5, 0.5, 0.49999, 0, 0, 1, 0.625, 1},        Vertex{-0.5, -0.5, 0.49999, 0, 0, 1, 0.375, 1},

    Vertex{-0.5, -0.5, -0.49999, 0, 0, -1, 0.375, 0.25},  Vertex{-0.5, 0.5, -0.49999, 0, 0, -1, 0.625, 0.25},
    Vertex{0.5, 0.5, -0.49999, 0, 0, -1, 0.625, 0.5},     Vertex{0.5, -0.5, -0.49999, 0, 0, -1, 0.375, 0.5},

    Vertex{-0.49999, -0.5, 0.5, -1, 0, 0, 0.375, 0},     Vertex{-0.49999, 0.5, 0.5, -1, 0, 0, 0.625, 0},
    Vertex{-0.49999, 0.5, -0.5, -1, 0, 0, 0.625, 0.25}, Vertex{-0.49999, -0.5, -0.5, -1, 0, 0, 0.375, 0.25},

    Vertex{0.49999, -0.5, -0.5, 1, 0, 0, 0.375, 0.5},   Vertex{0.49999, 0.5, -0.5, 1, 0, 0, 0.625, 0.5},
    Vertex{0.49999, 0.5, 0.5, 1, 0, 0, 0.625, 0.75},     Vertex{0.49999, -0.5, 0.5, 1, 0, 0, 0.375, 0.75},
};

RenderMeshBlocks::RenderMeshBlocks(const std::vector<FaceInfo>& blocks) : m_blocks(blocks) {
    // 0. generate vao, vbo, ebo
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo_vertices);
    glGenBuffers(1, &m_vbo_blocks);

    // 1. bind vao
    glBindVertexArray(m_vao);

    // 2. vbo_vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 3. vbo_blocks
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_blocks);
    glBufferData(GL_ARRAY_BUFFER, m_blocks.size() * sizeof(FaceInfo), &m_blocks[0].position.x, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // 4. m_model
    setModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));
}

RenderMeshBlocks::~RenderMeshBlocks() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo_vertices);
    glDeleteBuffers(1, &m_vbo_blocks);
}

void RenderMeshBlocks::draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) {
    shader->setUniform("u_model", model * m_model);
    if (m_shader_tag.find(shader.get()) == m_shader_tag.end()) { // 只初始化一次 u_cube
        m_shader_tag.insert(shader.get());
        shader->setUniform("u_cube", m_cube.size() * sizeof(Vertex) / sizeof(float), &m_cube[0].position.x);
        assert(m_cube.size() * sizeof(Vertex) / sizeof(float) == 192);
    }

    resource->getTexture("minecraft_texture")->use(shader, "u_block_texture", 0);

    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, m_blocks.size());
}

void RenderMeshBlocks::mapBuffer() {
    assert(m_ptr == nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_blocks);
    m_ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}
void RenderMeshBlocks::unmapBuffer() {
    assert(m_ptr != nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_blocks);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    m_ptr = nullptr;
}
void RenderMeshBlocks::updateBuffer(uint32_t pos, FaceInfo data) {
    assert(m_ptr != nullptr);
    memcpy(static_cast<float*>(m_ptr) + pos * sizeof(FaceInfo) / sizeof(float), &data.position.x, sizeof(FaceInfo));
}

} // namespace BJTUGE
