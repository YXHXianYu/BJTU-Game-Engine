#include "runtime/function/render/render_system.h"

#include <iostream>

#include <glad/glad.h>

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

    m_render_pipeline->draw(m_render_resource);

    glfwSwapBuffers(g_runtime_global_context.m_window_system->getWindow());
    glfwPollEvents();
}

void RenderSystem::clear() {}

} // namespace BJTUGE
