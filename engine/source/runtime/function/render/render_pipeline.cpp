#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/lighting/render_spot_light.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_framebuffer.h"
#include "runtime/function/render/render_shadow_framebuffer.h"
#include "runtime/function/render/render_gbuffer_framebuffer.h"
#include "runtime/function/render/render_texture_base.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// gbuffer
#include <gbuffer_block_frag.h>
#include <gbuffer_block_vert.h>
#include <gbuffer_textured_frag.h>
#include <gbuffer_textured_vert.h>
// shading
#include <shading_vert.h>
#include <shading_frag.h>
// postprocess
#include <postprocess_vert.h>
#include <composite0_frag.h>
#include <composite1_frag.h>
#include <composite2_frag.h>
#include <final_frag.h>
// shadow map
#include <shadow_map_vert.h>
#include <shadow_map_frag.h>
#include <shadow_map_block_vert.h>

#include <iostream>
#include <string>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    // === Shaders ===
    // gbuffer
    m_render_shaders["gbuffer_block"] = std::make_shared<RenderShader>(GBUFFER_BLOCK_VERT, GBUFFER_BLOCK_FRAG, "gbuffer_block");
    m_render_shaders["gbuffer_textured"] = std::make_shared<RenderShader>(GBUFFER_TEXTURED_VERT, GBUFFER_TEXTURED_FRAG, "gbuffer_textured");

    // shading
    m_render_shaders["shading"] = std::make_shared<RenderShader>(SHADING_VERT, SHADING_FRAG, "shading");

    // postprocess
    m_render_shaders["composite0"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE0_FRAG, "composite0");
    m_render_shaders["composite1"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE1_FRAG, "composite1");
    m_render_shaders["composite2"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE2_FRAG, "composite2");
    m_render_shaders["final"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, FINAL_FRAG, "final");

    // shadow map
    m_render_shaders["shadow_map"] = std::make_shared<RenderShader>(SHADOW_MAP_VERT, SHADOW_MAP_FRAG, "shadow_map");
    m_render_shaders["shadow_map_block"] = std::make_shared<RenderShader>(SHADOW_MAP_BLOCK_VERT, SHADOW_MAP_FRAG, "shadow_map_block");

    // === framebuffer ===
    int width = g_runtime_global_context.m_window_system->getWidth();
    int height = g_runtime_global_context.m_window_system->getHeight();

    m_render_framebuffers["shading"] = std::make_shared<RenderFramebuffer>(width, height);

    m_render_framebuffers["composite0"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite1"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite2"] = std::make_shared<RenderFramebuffer>(width, height);

    // g-buffer
    m_gbuffer_framebuffer = std::make_shared<RenderGBufferFramebuffer>(width, height);

    // shadow map
    m_shadow_framebuffer = std::make_shared<RenderShadowFramebuffer>(m_shadow_map_width, m_shadow_map_height);

    // === Matrix ===
    m_light_space_matrix = getLightSpaceMatrix();
}

// TODO: 这里可以将很多framebuffer的depth texture优化成render buffer来加速渲染
void RenderPipeline::draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    // change the position of the spot lights (should be moved to the logic in the future)
    {
        float    time = static_cast<float>(glfwGetTime());
        uint32_t i    = 0;
        for (auto& [key, spot_light] : resource->getModifiableSpotLights()) {
            spot_light->setPosition(glm::vec3(sin(time * i), i, cos(time)));
            i += 1;
        }
    }

    /* Shadow Map */
    if (m_is_enable_shadow_map) {
        draw_shadow_map(resource, camera);
    }

    /* G Buffer */
    draw_gbuffer(resource, camera);

    /* Shading */
    draw_shading(resource, camera);

    /* Post-process */
    draw_postprocess(resource, camera);
}

void RenderPipeline::draw_gbuffer(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    m_gbuffer_framebuffer->bind();

    // draw characters
    if (m_render_character || m_render_assignments || m_render_light) {
        auto shader = getShader("gbuffer_textured");
        shader->use();
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(m_use_ortho));

        if (m_render_character) {
            resource->getEntity("characters")->draw(shader, resource);
        }
        if (m_render_assignments) {
            resource->getEntity("assignments")->draw(shader, resource);
        }
        if (m_render_light) {
            for (const auto& [key, spot_light] : resource->getSpotLights()) {
                spot_light->draw(shader, resource);
            }
        }
    }
    
    // draw minecraft blocks
    if (m_render_block) {
        auto shader = getShader("gbuffer_block");
        shader->use();
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(m_use_ortho));
        resource->getEntity("minecraft_blocks")->draw(shader, resource);
    }

    m_gbuffer_framebuffer->unbind();
}

void RenderPipeline::draw_shading(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    getFramebuffer("shading")->bind();

    {
        auto shader = getShader("shading");
        shader->use();

        // gbuffer
        m_gbuffer_framebuffer->useGBufferPosition(shader, "u_gbuffer_position", 0);
        m_gbuffer_framebuffer->useGBufferNormal(shader, "u_gbuffer_normal", 1);
        m_gbuffer_framebuffer->useGBufferColor(shader, "u_gbuffer_color", 2);
        m_gbuffer_framebuffer->useDepthTexture(shader, "u_depth_texture", 3);

        // uniform
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_camera_position", camera->getPosition());

        // settings
        shader->setUniform("u_is_enable_depth_rendering", m_render_by_depth);

        // lights
        uint32_t i = 0;
        for (const auto& [key, spot_light] : resource->getSpotLights()) {
            auto&& index = std::to_string(i);
            shader->setUniform(("u_spotlights[" + index + "].pos").c_str(), spot_light->getPosition());
            shader->setUniform(("u_spotlights[" + index + "].color").c_str(), spot_light->getColor());
            i += 1;
        }
        shader->setUniform("u_spotlights_cnt", i);

        i = 0;
        for (const auto& [key, spot_light] : resource->getDirectionLights()) {
            auto&& index = std::to_string(i);
            shader->setUniform(("u_dirlights[" + index + "].dir").c_str(), spot_light->getDirection());
            shader->setUniform(("u_dirlights[" + index + "].color").c_str(), spot_light->getColor());
            i += 1;
        }
        shader->setUniform("u_dirlights_cnt", i);

        // shadow
        shader->setUniform("u_is_enable_shadow_map", m_is_enable_shadow_map);
        shader->setUniform("u_light_space_matrix", m_light_space_matrix);
        m_shadow_framebuffer->useDepthTexture(shader, "u_shadow_texture", 10);

        // render
        resource->getEntity("postprocess")->draw(shader, resource);
    }

    getFramebuffer("shading")->unbind();
}


void RenderPipeline::draw_shadow_map(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    glViewport(0, 0, m_shadow_map_width, m_shadow_map_height);
    m_shadow_framebuffer->bind();
    glCullFace(GL_FRONT); // to fix peter panning

    {
        auto shader = getShader("shadow_map");
        shader->use();

        shader->setUniform("u_light_space_matrix", m_light_space_matrix);

        if (m_render_character) {
            resource->getEntity("characters")->draw(shader, resource);
        }
        if (m_render_assignments) {
            resource->getEntity("assignments")->draw(shader, resource);
        }
    }

    {
        auto shader = getShader("shadow_map_block");
        shader->use();

        shader->setUniform("u_light_space_matrix", m_light_space_matrix);

        if (m_render_block) {
            resource->getEntity("minecraft_blocks")->draw(shader, resource);
        }
    }

    glCullFace(GL_BACK); // to fix peter panning
    m_shadow_framebuffer->unbind();
    glViewport(0, 0, g_runtime_global_context.m_window_system->getWidth(), g_runtime_global_context.m_window_system->getHeight());
}

void RenderPipeline::draw_postprocess(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    getFramebuffer("composite0")->bind();
    {
        auto shader = getShader("composite0");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_near", camera->getNear());
        shader->setUniform("u_far", camera->getFar());
        shader->setUniform("u_camera_position", camera->getPosition());
        shader->setUniform("u_inverse_view", camera->getInverseViewMatrix());
        shader->setUniform("u_inverse_projection", camera->getInverseProjectionMatrix(m_use_ortho));

        shader->setUniform("u_sunlight_direction",
            resource->getDirectionLights().begin()->second->getDirection()
        );

        getFramebuffer("shading")->useColorTexture(shader, "u_color_texture", 0);
        m_gbuffer_framebuffer->useDepthTexture(shader, "u_depth_texture", 1);
        m_gbuffer_framebuffer->useGBufferPosition(shader, "u_gbuffer_position", 2);
        resource->getTexture("noise_texture")->use(shader, "u_noise_texture", 3);


        resource->getEntity("postprocess")->draw(shader, resource);
    }
    getFramebuffer("composite0")->unbind();

    getFramebuffer("composite1")->bind();
    {
        auto shader = getShader("composite1");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        getFramebuffer("composite0")->useColorTexture(shader, "u_color_texture", 10);
        // getFramebuffer("composite0")->useDepthTexture(shader, "u_depth_texture", 11);

        resource->getEntity("postprocess")->draw(shader, resource);
    }
    getFramebuffer("composite1")->unbind();
    
    getFramebuffer("composite2")->bind();
    {
        auto shader = getShader("composite2");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        getFramebuffer("composite1")->useColorTexture(shader, "u_color_texture", 10);
        // getFramebuffer("composite1")->useDepthTexture(shader, "u_depth_texture", 11);

        resource->getEntity("postprocess")->draw(shader, resource);
    }
    getFramebuffer("composite2")->unbind();

    // draw to final framebuffer (default framebuffer)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        auto shader = getShader("final");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        getFramebuffer("composite0")->useColorTexture(shader, "u_origin_texture", 10);
        getFramebuffer("composite2")->useColorTexture(shader, "u_blur_texture", 11);
        // getFramebuffer("composite0")->useDepthTexture(shader, "u_depth_texture", ..);
        // getFramebuffer("composite2")->useDepthTexture(shader, "u_depth_texture", ..);

        resource->getEntity("postprocess")->draw(shader, resource);
    }
}

void RenderPipeline::tick(uint32_t GameCommand, std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_BLOCK)) {
        m_render_block = false;
    } else {
        m_render_block = true;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_CHARACTER)) {
        m_render_character = false;
    } else {
        m_render_character = true;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_LIGHT)) {
        m_render_light = false;
    } else {
        m_render_light = true;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::USE_ORTHO)) {
        m_use_ortho = true;
    } else {
        m_use_ortho = false;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_BY_DEPTH)) {
        m_render_by_depth = true;
    } else {
        m_render_by_depth = false;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::IS_ENABLE_SHADOW_MAP)) {
        m_is_enable_shadow_map = false;
    } else {
        m_is_enable_shadow_map = true;
    }

    draw(resource, camera);
}

glm::mat4 RenderPipeline::getLightSpaceMatrix() {
    glm::mat4 light_projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 50.0f);
    glm::mat4 light_view = glm::lookAt(glm::vec3(5.0f, 10.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_space_matrix = light_projection * light_view;
    return light_space_matrix;
}

std::shared_ptr<RenderShader>& RenderPipeline::getShader(const char* name) {
    if (m_render_shaders.find(name) == m_render_shaders.end()) {
        std::cerr << "Shader not found: " << name << std::endl;
        assert(false);
    }
    return m_render_shaders[name];
}
std::shared_ptr<RenderFramebuffer>& RenderPipeline::getFramebuffer(const char* name) {
    if (m_render_framebuffers.find(name) == m_render_framebuffers.end()) {
        std::cerr << "Framebuffer not found: " << name << std::endl;
        assert(false);
    }
    return m_render_framebuffers[name];
}

} // namespace BJTUGE
