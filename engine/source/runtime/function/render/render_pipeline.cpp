﻿#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/render/framebuffer/render_framebuffer.h"
#include "runtime/function/render/framebuffer/render_gbuffer_framebuffer.h"
#include "runtime/function/render/framebuffer/render_rsm_framebuffer.h"
#include "runtime/function/render/framebuffer/render_shadow_framebuffer.h"
#include "runtime/function/render/lighting/render_direction_light.h"
#include "runtime/function/render/lighting/render_spot_light.h"
#include "runtime/function/render/mesh/render_mesh.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/texture/render_texture_base.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// gbuffer
#include <gbuffer_block_frag.h>
#include <gbuffer_block_vert.h>
#include <gbuffer_textured_frag.h>
#include <gbuffer_textured_vert.h>
#include <gbuffer_transparent_frag.h>
#include <gbuffer_transparent_vert.h>
// rsm
#include <rsm_block_frag.h>
#include <rsm_block_vert.h>
#include <rsm_textured_frag.h>
#include <rsm_textured_vert.h>
// shading
#include <shading_frag.h>
#include <shading_vert.h>
// postprocess
#include <composite0_frag.h>
#include <composite1_frag.h>
#include <composite2_frag.h>
#include <composite3_frag.h>
#include <final_frag.h>
#include <final_fxaa_frag.h>
#include <postprocess_vert.h>
// shadow map
#include <shadow_map_block_vert.h>
#include <shadow_map_frag.h>
#include <shadow_map_vert.h>

#include <iostream>
#include <string>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    // === Shaders ===
    // gbuffer
    m_render_shaders["gbuffer_block"]    = std::make_shared<RenderShader>(GBUFFER_BLOCK_VERT, GBUFFER_BLOCK_FRAG, "gbuffer_block");
    m_render_shaders["gbuffer_textured"] = std::make_shared<RenderShader>(GBUFFER_TEXTURED_VERT, GBUFFER_TEXTURED_FRAG, "gbuffer_textured");
    m_render_shaders["gbuffer_transparent"] =
        std::make_shared<RenderShader>(GBUFFER_TRANSPARENT_VERT, GBUFFER_TRANSPARENT_FRAG, "gbuffer_transparent");

    // rsm
    m_render_shaders["rsm_block"]    = std::make_shared<RenderShader>(RSM_BLOCK_VERT, RSM_BLOCK_FRAG, "rsm_block");
    m_render_shaders["rsm_textured"] = std::make_shared<RenderShader>(RSM_TEXTURED_VERT, RSM_TEXTURED_FRAG, "rsm_textured");

    // shading
    m_render_shaders["shading"] = std::make_shared<RenderShader>(SHADING_VERT, SHADING_FRAG, "shading");

    // postprocess
    m_render_shaders["composite0"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE0_FRAG, "composite0");
    m_render_shaders["composite1"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE1_FRAG, "composite1");
    m_render_shaders["composite2"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE2_FRAG, "composite2");
    m_render_shaders["composite3"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE3_FRAG, "composite3");
    m_render_shaders["final"]      = std::make_shared<RenderShader>(POSTPROCESS_VERT, FINAL_FRAG, "final");
    m_render_shaders["final_fxaa"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, FINAL_FXAA_FRAG, "final_fxaa");

    // shadow map
    m_render_shaders["shadow_map"]       = std::make_shared<RenderShader>(SHADOW_MAP_VERT, SHADOW_MAP_FRAG, "shadow_map");
    m_render_shaders["shadow_map_block"] = std::make_shared<RenderShader>(SHADOW_MAP_BLOCK_VERT, SHADOW_MAP_FRAG, "shadow_map_block");

    // === framebuffer ===
    int width  = g_runtime_global_context.m_window_system->getWidth();
    int height = g_runtime_global_context.m_window_system->getHeight();

    m_render_framebuffers["shading"] = std::make_shared<RenderFramebuffer>(width, height);

    m_render_framebuffers["composite0"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite1"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite2"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite3"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["final"]      = std::make_shared<RenderFramebuffer>(width, height, true); // enable linear filter

    // g-buffer
    m_gbuffer_framebuffer = std::make_shared<RenderGBufferFramebuffer>(width, height);

    // shadow map
    m_shadow_framebuffer = std::make_shared<RenderShadowFramebuffer>(m_shadow_map_width, m_shadow_map_height);

    // rsm
    m_rsm_framebuffer = std::make_shared<RenderRsmFramebuffer>(m_rsm_width, m_rsm_height);

    // === Key bind ===
    bindKeyboardEvent();
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
    if (m_shadow_mode != 0) { draw_shadow_map(resource, camera); }

    /* G Buffer */
    draw_gbuffer(resource, camera);

    /* RSM */
    draw_rsm(resource, camera);

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

        if (m_render_character) { resource->getEntity("characters")->draw(shader, resource); }
        if (m_render_assignments) { resource->getEntity("assignments")->draw(shader, resource); }
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

    if (m_render_transparent) {
        auto shader = getShader("gbuffer_transparent");
        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_camera_position", camera->getPosition());
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(m_use_ortho));

        // 0:not water; 1:static; 2:normal with noise; 3:dynamic water
        shader->setUniform("u_water_mode", m_water_mode);

        uint32_t id = 0;
        resource->getTexture("noise_texture")->use(shader, "u_noise_texture", id++);

        shader->setUniform("u_transparent_info", glm::vec4(0.4, 0.6, 1.0, 0.6));
        resource->getEntity("water")->draw(shader, resource);
    }

    m_gbuffer_framebuffer->unbind();
}

void RenderPipeline::draw_rsm(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    if (!m_is_enable_rsm) return;

    glViewport(0, 0, m_rsm_width, m_rsm_height);
    m_rsm_framebuffer->bind();
    glCullFace(GL_FRONT); // to fix peter panning

    // draw characters
    if (m_render_character || m_render_assignments || m_render_light) {
        auto shader = getShader("rsm_textured");
        shader->use();
        shader->setUniform("u_view_projection", resource->getLightSpaceMatrix());

        if (m_render_character) { resource->getEntity("characters")->draw(shader, resource); }
        if (m_render_assignments) { resource->getEntity("assignments")->draw(shader, resource); }
        if (m_render_light) { /* NO NEED */
        }
    }

    // draw minecraft blocks
    if (m_render_block) {
        auto shader = getShader("rsm_block");
        shader->use();
        shader->setUniform("u_view_projection", resource->getLightSpaceMatrix());

        resource->getEntity("minecraft_blocks")->draw(shader, resource);
    }

    if (m_render_transparent) { /* NO NEED */
    }

    glCullFace(GL_BACK); // to fix peter panning
    m_rsm_framebuffer->unbind();
    glViewport(0, 0, g_runtime_global_context.m_window_system->getWidth(), g_runtime_global_context.m_window_system->getHeight());
}

void RenderPipeline::draw_shading(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    getFramebuffer("shading")->bind();

    {
        auto shader = getShader("shading");
        shader->use();

        size_t id = 0;

        // gbuffer
        m_gbuffer_framebuffer->useGBufferPosition(shader, "u_gbuffer_position", id++);
        m_gbuffer_framebuffer->useGBufferNormal(shader, "u_gbuffer_normal", id++);
        m_gbuffer_framebuffer->useGBufferColor(shader, "u_gbuffer_color", id++);
        m_gbuffer_framebuffer->useGBufferTransparent(shader, "u_gbuffer_transparent", id++);
        m_gbuffer_framebuffer->useDepthTexture(shader, "u_depth_texture", id++);

        // rsm
        shader->setUniform("u_is_enable_rsm", m_is_enable_rsm);
        if (m_is_enable_rsm) {
            shader->setUniform("u_rsm_width", m_rsm_framebuffer->getWidth());
            shader->setUniform("u_rsm_height", m_rsm_framebuffer->getHeight());
            m_rsm_framebuffer->usePositionTexture(shader, "u_rsm_position_texture", id++);
            m_rsm_framebuffer->useNormalTexture(shader, "u_rsm_normal_texture", id++);
            m_rsm_framebuffer->useColorTexture(shader, "u_rsm_color_texture", id++);
        }

        // uniform
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_camera_position", camera->getPosition());

        // rendering mode
        shader->setUniform("u_is_enable_depth_rendering", m_render_by_depth); // TODO: Broken

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
        shader->setUniform("u_shadow_mode", m_shadow_mode);
        m_shadow_framebuffer->useDepthTexture(shader, "u_shadow_texture", id++);
        shader->setUniform("u_shadow_map_width", static_cast<float>(m_shadow_map_width));
        shader->setUniform("u_shadow_map_height", static_cast<float>(m_shadow_map_height));

        shader->setUniform("u_light_space_matrix", resource->getLightSpaceMatrix());
        shader->setUniform("u_light_space_near", resource->getLightSpaceNear());
        shader->setUniform("u_light_space_far", resource->getLightSpaceFar());

        // render
        resource->getEntity("postprocess")->draw(shader, resource);
    }

    getFramebuffer("shading")->unbind();
}

void RenderPipeline::draw_shadow_map(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    glViewport(0, 0, m_shadow_map_width, m_shadow_map_height);
    m_shadow_framebuffer->bind();
    glCullFace(GL_FRONT); // to fix peter panning

    // If shadow map has some artifacts, try to change the light space matrix in RenderResource::updateLightSpaceMatrix()

    {
        auto shader = getShader("shadow_map");
        shader->use();

        shader->setUniform("u_light_space_matrix", resource->getLightSpaceMatrix());
        shader->setUniform("u_near", resource->getLightSpaceNear());
        shader->setUniform("u_far", resource->getLightSpaceFar());

        if (m_render_character) { resource->getEntity("characters")->draw(shader, resource); }
        if (m_render_assignments) { resource->getEntity("assignments")->draw(shader, resource); }
    }

    {
        auto shader = getShader("shadow_map_block");
        shader->use();

        shader->setUniform("u_light_space_matrix", resource->getLightSpaceMatrix());
        shader->setUniform("u_near", resource->getLightSpaceNear());
        shader->setUniform("u_far", resource->getLightSpaceFar());

        if (m_render_block) { resource->getEntity("minecraft_blocks")->draw(shader, resource); }
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

        shader->setUniform("u_sunlight_direction", resource->getDirectionLights().begin()->second->getDirection());
        shader->setUniform("u_cloud_size", m_cloud_thickness); // 云层厚度,范围从0~0.6f
        shader->setUniform("u_sky_color", m_sky_color);        // 天空的颜色修改

        uint32_t id = 0;
        getFramebuffer("shading")->useColorTexture(shader, "u_color_texture", id++);
        m_gbuffer_framebuffer->useDepthTexture(shader, "u_depth_texture", id++);
        m_gbuffer_framebuffer->useGBufferPosition(shader, "u_gbuffer_position", id++);
        m_gbuffer_framebuffer->useGBufferNormal(shader, "u_gbuffer_normal", id++);
        m_gbuffer_framebuffer->useGBufferTransparent(shader, "u_gbuffer_transparent", id++);
        resource->getTexture("noise_texture")->use(shader, "u_noise_texture", id++);

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
        shader->setUniform("u_near", camera->getNear());
        shader->setUniform("u_far", camera->getFar());
        shader->setUniform("u_camera_position", camera->getPosition());
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(m_use_ortho));

        uint32_t id = 0;
        getFramebuffer("composite0")->useColorTexture(shader, "u_color_texture", id++);
        m_gbuffer_framebuffer->useDepthTexture(shader, "u_depth_texture", id++);
        m_gbuffer_framebuffer->useGBufferPosition(shader, "u_gbuffer_position", id++);
        m_gbuffer_framebuffer->useGBufferNormal(shader, "u_gbuffer_normal", id++);
        m_gbuffer_framebuffer->useGBufferTransparent(shader, "u_gbuffer_transparent", id++);

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

    getFramebuffer("composite3")->bind();
    {
        auto shader = getShader("composite3");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        getFramebuffer("composite2")->useColorTexture(shader, "u_color_texture", 10);
        // getFramebuffer("composite2")->useDepthTexture(shader, "u_depth_texture", 11);

        resource->getEntity("postprocess")->draw(shader, resource);
    }
    getFramebuffer("composite3")->unbind();

    getFramebuffer("final")->bind();
    {
        auto shader = getShader("final");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        uint32_t id = 0;
        getFramebuffer("composite1")->useColorTexture(shader, "u_origin_texture", id++);
        getFramebuffer("composite3")->useColorTexture(shader, "u_blur_texture", id++);

        resource->getEntity("postprocess")->draw(shader, resource);
    }
    getFramebuffer("final")->unbind();

    // draw to final framebuffer (default framebuffer)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        auto shader = getShader("final_fxaa");

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_fxaa_mode", m_fxaa_mode);

        uint32_t id = 0;
        // m_gbuffer_framebuffer->useDepthTexture(shader, "u_color_texture", id++);
        getFramebuffer("final")->useColorTexture(shader, "u_color_texture", id++);

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

    draw(resource, camera);
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

void RenderPipeline::bindKeyboardEvent() {
    g_runtime_global_context.m_window_system->registerOnKeyFunc(
        std::bind(&RenderPipeline::onKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

// TODO:添加新的事件
void RenderPipeline::onKey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            default: {
                break;
            }
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_N: {
                m_water_mode = m_water_mode % 5 + 1;
                break;
            }
            case GLFW_KEY_M: {
                m_fxaa_mode = (m_fxaa_mode + 1) % 5; // 0: off; 1: blend; 2: edge blend; 3: 十字滤波; 4: 彩色十字滤波
                break;
            }
            case GLFW_KEY_COMMA: {
                m_shadow_mode =
                    (m_shadow_mode + 1) % 5; // 0: off; 1: hard shadow; 2: soft shadow (PCF); 3: soft shadow (PCSS); 4: soft shadow (VSM)
                break;
            }
            case GLFW_KEY_Z: {
                if (m_cloud_thickness < 0.6) {
                    m_cloud_thickness += 0.05f;
                    m_sky_color += glm::vec3(0.04, 0.02, 0);
                }
                break;
            }
            case GLFW_KEY_X: {
                if (m_cloud_thickness > 0) {
                    m_cloud_thickness -= 0.05f;
                    m_sky_color -= glm::vec3(0.04, 0.02, 0);
                }
                break;
            }
            case GLFW_KEY_C: {
                m_is_enable_rsm ^= 1;
                break;
            }

            default: {
                break;
            }
        }
    }
}

} // namespace BJTUGE
