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

// normal
#include <block_frag.h>
#include <block_vert.h>
#include <depth_frag.h>
#include <depth_vert.h>
#include <light_frag.h>
#include <model_frag.h>
#include <model_vert.h>
// postprocess
#include <postprocess_vert.h>
#include <composite1_frag.h>

#include "render_pipeline.h"
#include <iostream>
#include <string>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    m_render_shaders["depth"] = std::make_shared<RenderShader>(DEPTH_VERT, DEPTH_FRAG);
    m_render_shaders["block"] = std::make_shared<RenderShader>(BLOCK_VERT, BLOCK_FRAG);
    m_render_shaders["model"] = std::make_shared<RenderShader>(MODEL_VERT, MODEL_FRAG);
    m_render_shaders["light"] = std::make_shared<RenderShader>(MODEL_VERT, LIGHT_FRAG);

    m_render_shaders["composite1"] = std::make_shared<RenderShader>(POSTPROCESS_VERT, COMPOSITE1_FRAG);

    int width = g_runtime_global_context.m_window_system->getWidth();
    int height = g_runtime_global_context.m_window_system->getHeight();
    m_render_framebuffers["origin"] = std::make_shared<RenderFramebuffer>(width, height);
}

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

    // bind to origin framebuffer
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

    // unbind
    m_render_framebuffers["origin"]->unbind();

    {
        auto shader = m_render_shaders["composite1"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));

        m_render_framebuffers["origin"]->useColorTexture(shader, "u_color_texture", 10);
        m_render_framebuffers["origin"]->useDepthTexture(shader, "u_depth_texture", 11);

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
