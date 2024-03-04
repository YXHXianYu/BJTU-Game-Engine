#pragma once

#include <functional>

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

    /**
     * @brief Set the resize callback object
     *
     * @param callback the resize callback function, type is void(GLFWwindow*, int, int)
     */
    void setResizeCallback(GLFWframebuffersizefun callback);

    /**
     * @brief Check if the window should close
     */
    int windowShouldClose() const { return m_window_should_close; }

private:
    GLFWwindow* m_window{nullptr};
    int         m_width{0};
    int         m_height{0};
    int         m_window_should_close{false};
};

} // namespace BJTUGE