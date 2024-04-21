#include "runtime/function/window/window_system.h"

#include <cassert>
#include <iostream>

namespace BJTUGE {

void WindowSystem::initialize(WindowCreateInfo create_info) {
    // ===== Window System =====
    // config
    m_width  = create_info.width;
    m_height = create_info.height;

    // glfw initialize
    if (!glfwInit()) { assert(false); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
    // glViewport(0, 0, m_width, m_height);
    // glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });

    // ===== Event System =====
    // initialize event system
    glfwSetWindowUserPointer(m_window, this);

    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetFramebufferSizeCallback(m_window, resizeCallback);

    glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

    // window
    registerOnKeyFunc(std::bind(&WindowSystem::onKeyEscape, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                std::placeholders::_4));
    registerOnResizeFunc(std::bind(&WindowSystem::onResizeFramebuffer, this, std::placeholders::_1, std::placeholders::_2));
}

void WindowSystem::tick(float delta_time) {
    // glfwGetWindowSize(m_window, &m_width, &m_height); // This command has bug in HyprLand in Linux Platform. Fk
    m_window_should_close = glfwWindowShouldClose(m_window);
}

void WindowSystem::clear() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WindowSystem::onKeyEscape(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(m_window, true); }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) { glfwSetWindowShouldClose(m_window, true); }
}

void WindowSystem::onResizeFramebuffer(int width, int height) {
    m_width  = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

} // namespace BJTUGE
