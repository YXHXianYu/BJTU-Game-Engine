#include "runtime/function/render/render_mesh_blocks.h"

#include "runtime/function/render/render_shader.h"

namespace BJTUGE {

const std::vector<float> RenderMeshBlocks::m_vertices = {
    0.0f, 1.0f, 2.0f, 3.0f
};

RenderMeshBlocks::RenderMeshBlocks(const std::vector<BlockInfo> blocks)
    : m_blocks(blocks) {
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
    glBufferData(GL_ARRAY_BUFFER, m_blocks.size() * sizeof(BlockInfo), &m_blocks[0].position.x, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
}

RenderMeshBlocks::~RenderMeshBlocks() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo_vertices);
    glDeleteBuffers(1, &m_vbo_blocks);
}

void RenderMeshBlocks::draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) {
    shader->setUniform("u_model", model * m_model);

    // todo: apply texture

    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 100);
}

}

