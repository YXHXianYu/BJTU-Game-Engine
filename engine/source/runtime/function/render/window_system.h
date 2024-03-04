#pragma once

#include <GLFW/glfw3.h>

namespace BJTUGE {

class WindowSystem {
public:
    void initialize();
    void tick(float delta_time);
    void clear();

    GLFWwindow* getWindow() const;
};

} // namespace BJTUGE