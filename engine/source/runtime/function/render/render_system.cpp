#include "runtime/function/render/render_system.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_pipeline.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

namespace BJTUGE {

void RenderSystem::initialize() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(true);

    m_render_resource = std::make_shared<RenderResource>();
    m_render_resource->initialize();

    m_render_pipeline = std::make_shared<RenderPipeline>();
    m_render_pipeline->initialize();

    m_render_camera = std::make_shared<RenderCamera>(CameraCreateInfo{});
    m_render_camera->setAspect(g_runtime_global_context.m_window_system->getAspect());

    std::cout << "RenderSystem::initialize complete" << std::endl;
}

void RenderSystem::tick(float delta_time) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto input_system = g_runtime_global_context.m_input_system;
    m_render_camera->tick(delta_time, input_system->getGameCommand(), input_system->getCursorDeltaX(), input_system->getCursorDeltaY());
    input_system->resetCursorDelta();

    m_render_pipeline->draw(m_render_resource, m_render_camera);

    glfwSwapBuffers(g_runtime_global_context.m_window_system->getWindow());
    glfwPollEvents();
}

void RenderSystem::clear() {}

} // namespace BJTUGE
