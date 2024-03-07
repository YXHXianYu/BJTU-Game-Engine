#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <basic_vert.h>
#include <basic_frag.h>
#include <post_process_vert.h>
// #include <shadertoy_clouds_frag.h>
#include <shadertoy_slisesix_frag.h>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    m_render_shaders["basic"]     = std::make_shared<RenderShader>(BASIC_VERT, BASIC_FRAG);
    m_render_shaders["shadertoy"] = std::make_shared<RenderShader>(POST_PROCESS_VERT, SHADERTOY_SLISESIX_FRAG);
}

void RenderPipeline::draw(std::shared_ptr<RenderResource> render_resource) {
    auto shader = m_render_shaders["basic"];

    // shader program
    shader->use();
    shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
    shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                       static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

    // texture
    shader->setTexture("u_texture0", 0, render_resource->get("basic").m_render_textures[0]);
    shader->setTexture("u_texture1", 1, render_resource->get("basic").m_render_textures[1]);

    // draw

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::rotate(model, glm::radians(-45.0f + 10.0f * float(glfwGetTime())), glm::vec3(1.0, 2.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view           = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.5f));

    glm::mat4 projection = glm::mat4(1.0f);
    projection           = glm::perspective(glm::radians(90.0f),
                                            static_cast<float>(g_runtime_global_context.m_window_system->getWidth()) /
                                                static_cast<float>(g_runtime_global_context.m_window_system->getHeight()),
                                            0.1f, 1000.0f);

    shader->setUniform("model", model);
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);

    render_resource->get("basic").m_render_meshes[1]->use();
    render_resource->get("basic").m_render_meshes[1]->draw();
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
