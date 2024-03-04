#include "runtime/engine.h"

#include <iostream>
#include <memory>

#include <GLFW/glfw3.h>

int main() {

    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    std::unique_ptr<BJTUGE::BJTUEngine> engine = std::make_unique<BJTUGE::BJTUEngine>();

    std::cout << "Starting engine." << std::endl;

    engine->startEngine();

    std::cout << "Engine is started." << std::endl;
    std::cout << "Engine is running." << std::endl;

    engine->run();

    std::cout << "Shuting down engine." << std::endl;

    engine->shutdownEngine();

    std::cout << "Engine is shut down." << std::endl;

    return 0;
}