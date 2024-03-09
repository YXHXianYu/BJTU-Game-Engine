#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace BJTUGE {

struct WindowCreateInfo {
    int         width{1280};
    int         height{720};
    const char* title{"BJTU Game Engine"};
    bool        is_fullscreen{false};
};

class WindowSystem {
public:
    void initialize(WindowCreateInfo create_info);
    void tick(float delta_time);
    void clear();

    /**
     * @brief Get the window object
     */
    GLFWwindow* getWindow() const { return m_window; }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    float    getAspect() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

    /**
     * @brief Set the resize callback object
     * @param callback the resize callback function, type is void(GLFWwindow*, int, int)
     */
    void setResizeCallback(GLFWframebuffersizefun callback);

    /**
     * @brief Check if the window should close
     * @return int 1 if the window should close, 0 otherwise
     */
    int windowShouldClose() const { return m_window_should_close; }

    // bool getIsFocusMode() const { return m_is_focus_mode; }
    // void setIsFocusMode(bool is_focus_mode) {
    //     m_is_focus_mode = is_focus_mode;
    //     glfwSetInputMode(m_window, GLFW_CURSOR, m_is_focus_mode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    // }

private:
    void onKeyEscape(int key, int scancode, int action, int mods);
    void onResizeFramebuffer(int width, int height);

private:
    GLFWwindow* m_window{nullptr};
    int         m_width{0};
    int         m_height{0};
    int         m_window_should_close{false};

    bool m_is_focus_mode{false};

    // ===== ===== Event System ===== =====

public:
    typedef std::function<void(int, int, int, int)> onKeyFunc;
    typedef std::function<void(double, double)>     onCursorPosFunc;
    typedef std::function<void(int, int, int)>      onMouseButtonFunc;
    typedef std::function<void(double, double)>     onScrollFunc;
    typedef std::function<void(int, int)>           onResizeFunc;

    void registerOnKeyFunc(onKeyFunc func) { m_on_key_func.push_back(func); }
    void registerOnCursorPosFunc(onCursorPosFunc func) { m_on_cursor_pos_func.push_back(func); }
    void registerOnMouseButtonFunc(onMouseButtonFunc func) { m_on_mouse_button_func.push_back(func); }
    void registerOnScrollFunc(onScrollFunc func) { m_on_scroll_func.push_back(func); }
    void registerOnResizeFunc(onResizeFunc func) { m_on_resize_func.push_back(func); }

    void onKey(int key, int scancode, int action, int mods) {
        for (auto& func : m_on_key_func)
            func(key, scancode, action, mods);
    }
    void onCursorPos(double xpos, double ypos) {
        for (auto& func : m_on_cursor_pos_func)
            func(xpos, ypos);
    }
    void onMouseButton(int button, int action, int mods) {
        for (auto& func : m_on_mouse_button_func)
            func(button, action, mods);
    }
    void onScroll(double xoffset, double yoffset) {
        for (auto& func : m_on_scroll_func)
            func(xoffset, yoffset);
    }
    void onResize(int width, int height) {
        for (auto& func : m_on_resize_func)
            func(width, height);
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
        if (app) { app->onKey(key, scancode, action, mods); }
    }
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
        if (app) { app->onCursorPos(xpos, ypos); }
    }
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
        if (app) { app->onMouseButton(button, action, mods); }
    }
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
        if (app) { app->onScroll(xoffset, yoffset); }
    }
    static void resizeCallback(GLFWwindow* window, int width, int height) {
        WindowSystem* app = (WindowSystem*)glfwGetWindowUserPointer(window);
        if (app) { app->onResize(width, height); }
    }

private:
    std::vector<onKeyFunc>         m_on_key_func;
    std::vector<onCursorPosFunc>   m_on_cursor_pos_func;
    std::vector<onMouseButtonFunc> m_on_mouse_button_func;
    std::vector<onScrollFunc>      m_on_scroll_func;
    std::vector<onResizeFunc>      m_on_resize_func;

    // 添加新时间时，记得在 "window_system.cpp - initialize()" 中添加相应的注册函数
};

} // namespace BJTUGE
