#include "runtime/function/render/render_system.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "runtime/function/global/global_context.h"

namespace BJTUGE {

void RenderSystem::initialize() {}

void RenderSystem::tick(float delta_time) {
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(g_runtime_global_context.m_window_system->getWindow());
    glfwPollEvents();
}

void RenderSystem::clear() {}

} // namespace BJTUGE