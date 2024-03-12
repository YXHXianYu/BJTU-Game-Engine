#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/window/window_system.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <basic_vert.h>
#include <basic_frag.h>
#include <model_vert.h>
#include <model_frag.h>
#include <depth_rendering_vert.h>
#include <depth_rendering_frag.h>

#include <iostream>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    m_render_shaders["basic"]    = std::make_shared<RenderShader>(BASIC_VERT, BASIC_FRAG);
    m_render_shaders["3d-model"] = std::make_shared<RenderShader>(MODEL_VERT, MODEL_FRAG);
    m_render_shaders["depth"]    = std::make_shared<RenderShader>(DEPTH_RENDERING_VERT, DEPTH_RENDERING_FRAG);
}

void RenderPipeline::draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    {
        auto shader = m_render_shaders["3d-model"];
        // auto shader = m_render_shaders["depth"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix());

        resource->getEntity("aris")->draw(shader, resource);
        resource->getEntity("miyako")->draw(shader, resource);
        resource->getEntity("koharu")->draw(shader, resource);
        resource->getEntity("block")->draw(shader, resource);
    }

    return;

    {
        auto shader = m_render_shaders["basic"];
        auto cube   = resource->getEntity("cube");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix());

        cube->setModelMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f + 10.0f * float(glfwGetTime())), glm::vec3(1.0, 2.0f, 0.0f)));

        cube->draw(shader, resource);
    }
}

} // namespace BJTUGE
