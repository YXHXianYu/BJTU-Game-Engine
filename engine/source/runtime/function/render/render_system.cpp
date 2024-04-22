#include "runtime/function/render/render_system.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/render/minecraft_blocks/gmemory_buffer.h"
#include "runtime/function/render/minecraft_blocks/render_minecraft_blocks_manager.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_pipeline.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/window/window_system.h"

#include "runtime/function/swap/swap_context.h"
#include "runtime/function/swap/swap_event.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

namespace BJTUGE {

void RenderSystem::initialize() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
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

    // input
    auto input_system = g_runtime_global_context.m_input_system;
    m_render_camera->tick(delta_time, input_system->getGameCommand(), input_system->getCursorDeltaX(), input_system->getCursorDeltaY());
    input_system->resetCursorDelta();

    // swap context from logical module
    swap();
    // draw
    m_render_pipeline->tick(input_system->getGameCommand(), m_render_resource, m_render_camera);

    // post tick
    glfwSwapBuffers(g_runtime_global_context.m_window_system->getWindow());
    glfwPollEvents();
}

void RenderSystem::clear() {}

void RenderSystem::swap() {
    m_render_resource->getRenderMinecraftBlocksManager()->startTransfer();

    while (!g_runtime_global_context.m_swap_context->isEmpty()) {
        auto event = g_runtime_global_context.m_swap_context->pop();
        event->update(m_render_resource.get());
    }

    m_render_resource->getRenderMinecraftBlocksManager()->endTransfer();
}

} // namespace BJTUGE
