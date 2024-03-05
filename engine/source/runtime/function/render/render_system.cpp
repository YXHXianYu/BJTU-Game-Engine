#include "runtime/function/render/render_system.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "runtime/function/global/global_context.h"

#include <iostream>

namespace BJTUGE {

void RenderSystem::initialize() {
    m_render_resource = std::make_shared<RenderResource>();
    m_render_resource->initialize();

    m_render_pipeline = std::make_shared<RenderPipeline>();
    m_render_pipeline->initialize();
}

void RenderSystem::tick(float delta_time) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    uint32_t shader_program = m_render_pipeline->getShaderProgram();

    static bool first_run = false;

    for (auto& [uuid, entity] : *m_render_resource) {
        uint32_t vao = entity.m_render_meshes[0].getVAO();

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(g_runtime_global_context.m_window_system->getWindow());
    glfwPollEvents();
}

void RenderSystem::clear() {}

} // namespace BJTUGE
