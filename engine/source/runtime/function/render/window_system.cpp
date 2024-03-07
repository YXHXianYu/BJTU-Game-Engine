#include "runtime/function/render/window_system.h"

#include <iostream>
#include <cassert>

namespace BJTUGE {

void WindowSystem::initialize(WindowCreateInfo create_info) {
    // config
    m_width  = create_info.width;
    m_height = create_info.height;

    // glfw initialize
    if (!glfwInit()) { assert(false); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // window initialize
    m_window = glfwCreateWindow(m_width, m_height, create_info.title, NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        assert(false);
    }
    glfwMakeContextCurrent(m_window);

    // glad initialize
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { assert(false); }

    // viewport
    glViewport(0, 0, m_width, m_height);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });
}

void WindowSystem::tick(float delta_time) {
    glfwGetWindowSize(m_window, &m_width, &m_height);
    m_window_should_close = glfwWindowShouldClose(m_window);
}

void WindowSystem::clear() {
    glfwTerminate();
}

void WindowSystem::setResizeCallback(GLFWframebuffersizefun callback) {
    glfwSetFramebufferSizeCallback(m_window, callback);
}

} // namespace BJTUGE
