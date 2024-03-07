#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
    // shader program
    m_render_shaders["basic"]->use();
    m_render_shaders["basic"]->setUniform("u_time", static_cast<float>(glfwGetTime()));
    m_render_shaders["basic"]->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                                          static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
    // mesh
    render_resource->get("basic").m_render_meshes[0]->use();
    // texture 0
    render_resource->get("basic").m_render_textures[0]->use();
    m_render_shaders["basic"]->setUniform("u_texture0", render_resource->get("basic").m_render_textures[0]->getId());
    // texture 1
    render_resource->get("basic").m_render_textures[1]->use();
    m_render_shaders["basic"]->setUniform("u_texture1", render_resource->get("basic").m_render_textures[1]->getId());
    // draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
