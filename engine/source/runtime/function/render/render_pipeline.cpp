#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/window/window_system.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <basic_vert.h>
#include <basic_frag.h>
#include <post_process_vert.h>
#include <shadertoy_slisesix_frag.h>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    m_render_shaders["basic"]     = std::make_shared<RenderShader>(BASIC_VERT, BASIC_FRAG);
    m_render_shaders["shadertoy"] = std::make_shared<RenderShader>(POST_PROCESS_VERT, SHADERTOY_SLISESIX_FRAG);
}

void RenderPipeline::draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    auto shader = m_render_shaders["basic"];
    auto entity = resource->get("basic");

    // shader program
    shader->use();
    shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
    shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                       static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

    // texture
    shader->setTexture("u_texture0", 0, resource->get("basic").getRenderTextures()[0]);
    shader->setTexture("u_texture1", 1, resource->get("basic").getRenderTextures()[1]);

    // tick
    entity.setModelMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f + 10.0f * float(glfwGetTime())), glm::vec3(1.0, 2.0f, 0.0f)));

    // mvp
    shader->setUniform("model", entity.getModelMatrix());
    shader->setUniform("view", camera->getViewMatrix());
    shader->setUniform("projection", camera->getProjectionMatrix());

    // draw
    entity.getRenderMeshes()[1]->draw();
}

void RenderPipeline::drawShadertoy() {
    assert(false);

    m_render_shaders["shadertoy"]->use();
    m_render_shaders["shadertoy"]->setUniform("u_time", static_cast<float>(glfwGetTime()));
    m_render_shaders["shadertoy"]->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                                              static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
    m_render_shaders["shadertoy"]->setUniform("u_mouse", 0.0f, 0.0f);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

} // namespace BJTUGE
