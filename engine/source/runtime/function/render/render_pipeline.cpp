#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/lighting/render_spot_light.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_framebuffer.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// origin
#include <block_frag.h>
#include <block_vert.h>
#include <depth_frag.h>
#include <depth_vert.h>
#include <light_frag.h>
#include <model_frag.h>
#include <model_vert.h>
// postprocess
#include <postprocess_vert.h>
#include <composite0_frag.h>
#include <composite1_frag.h>
#include <composite2_frag.h>
#include <final_frag.h>

#include "render_pipeline.h"
#include <iostream>
#include <string>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    // origin
    m_render_shaders["depth"] = std::make_shared<RenderShader>(DEPTH_VERT, DEPTH_FRAG);
    m_render_shaders["block"] = std::make_shared<RenderShader>(BLOCK_VERT, BLOCK_FRAG);
    m_render_shaders["model"] = std::make_shared<RenderShader>(MODEL_VERT, MODEL_FRAG);
    m_render_shaders["light"] = std::make_shared<RenderShader>(MODEL_VERT, LIGHT_FRAG);

    // postprocess (composite)
    m_render_shaders["composite0"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE0_FRAG);
    m_render_shaders["composite1"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE1_FRAG);
    m_render_shaders["composite2"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE2_FRAG);
    m_render_shaders["final"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, FINAL_FRAG);

    // framebuffer
    int width = g_runtime_global_context.m_window_system->getWidth();
    int height = g_runtime_global_context.m_window_system->getHeight();

    m_render_framebuffers["origin"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite0"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite1"] = std::make_shared<RenderFramebuffer>(width, height);
    m_render_framebuffers["composite2"] = std::make_shared<RenderFramebuffer>(width, height);
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

    m_render_framebuffers["origin"]->bind();

    // draw characters
    if (render_character || render_assignments) {
        auto shader = m_render_shaders["model"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(use_ortho));
        shader->setUniform("u_cam_pos", camera->getPosition());
        shader->setUniform("u_render_by_depth", render_by_depth);

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

        if (render_character) {
            resource->getEntity("characters")->draw(shader, resource);
        }
        if (render_assignments) {
            resource->getEntity("assignments")->draw(shader, resource);
        }
    }

    // draw light mesh
    if (render_light) {
        auto light_shader = m_render_shaders["light"];
        light_shader->use();
        light_shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        light_shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                                 static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        light_shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(use_ortho));

        uint32_t i = 0;
        for (const auto& [key, spot_light] : resource->getSpotLights()) {
            auto&& index = std::to_string(i);
            light_shader->setUniform("u_light_color", spot_light->getColor());
            spot_light->draw(light_shader, resource);
            i += 1;
        }
    }
    
    // draw minecraft blocks
    if (render_block) {
        auto shader = m_render_shaders["block"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(use_ortho));
        shader->setUniform("u_cam_pos", camera->getPosition());
        shader->setUniform("u_render_by_depth", render_by_depth);

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

        resource->getEntity("minecraft_blocks")->draw(shader, resource);
    }

    m_render_framebuffers["origin"]->unbind();
    m_render_framebuffers["composite0"]->bind();

    {
        auto shader = m_render_shaders["composite0"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        m_render_framebuffers["origin"]->useColorTexture(shader, "u_color_texture", 10);
        m_render_framebuffers["origin"]->useDepthTexture(shader, "u_depth_texture", 11);

        resource->getEntity("postprocess")->draw(shader, resource);
    }

    m_render_framebuffers["composite0"]->unbind();
    m_render_framebuffers["composite1"]->bind();

    {
        auto shader = m_render_shaders["composite1"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        m_render_framebuffers["composite0"]->useColorTexture(shader, "u_color_texture", 10);
        // m_render_framebuffers["composite0"]->useDepthTexture(shader, "u_depth_texture", 11);

        resource->getEntity("postprocess")->draw(shader, resource);
    }

    m_render_framebuffers["composite1"]->unbind();
    m_render_framebuffers["composite2"]->bind();

    {
        auto shader = m_render_shaders["composite2"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        m_render_framebuffers["composite1"]->useColorTexture(shader, "u_color_texture", 10);
        // m_render_framebuffers["composite1"]->useDepthTexture(shader, "u_depth_texture", 11);

        resource->getEntity("postprocess")->draw(shader, resource);
    }

    m_render_framebuffers["composite2"]->unbind();

    // draw to final framebuffer (default framebuffer)
    {
        auto shader = m_render_shaders["final"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        m_render_framebuffers["composite0"]->useColorTexture(shader, "u_origin_texture", 10);
        m_render_framebuffers["composite2"]->useColorTexture(shader, "u_blur_texture", 11);
        // m_render_framebuffers["composite0"]->useDepthTexture(shader, "u_depth_texture", ..);
        // m_render_framebuffers["composite2"]->useDepthTexture(shader, "u_depth_texture", ..);

        resource->getEntity("postprocess")->draw(shader, resource);
    }

}

void RenderPipeline::tick(uint32_t GameCommand, std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_BLOCK)) {
        render_block = false;
    } else {
        render_block = true;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_CHARACTER)) {
        render_character = false;
    } else {
        render_character = true;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_LIGHT)) {
        render_light = false;
    } else {
        render_light = true;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::USE_ORTHO)) {
        use_ortho = true;
    } else {
        use_ortho = false;
    }

    if (GameCommand & static_cast<uint32_t>(GameCommand::RENDER_BY_DEPTH)) {
        render_by_depth = true;
    } else {
        render_by_depth = false;
    }

    draw(resource, camera);
}
} // namespace BJTUGE
