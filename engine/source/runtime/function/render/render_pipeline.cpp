#include "runtime/function/render/render_pipeline.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/lighting/render_spot_light.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_resource.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <basic_frag.h>
#include <basic_vert.h>
#include <block_frag.h>
#include <block_vert.h>
#include <depth_rendering_frag.h>
#include <depth_rendering_vert.h>
#include <light_frag.h>
#include <model_frag.h>
#include <model_vert.h>
#include <model_with_lighting_frag.h>
#include <model_with_lighting_vert.h>

#include <iostream>
#include <string>

namespace BJTUGE {

unsigned int texture1, texture2;

void RenderPipeline::initialize() {
    m_render_shaders["model"]               = std::make_shared<RenderShader>(MODEL_VERT, MODEL_FRAG);
    m_render_shaders["depth"]               = std::make_shared<RenderShader>(DEPTH_RENDERING_VERT, DEPTH_RENDERING_FRAG);
    m_render_shaders["block"]               = std::make_shared<RenderShader>(BLOCK_VERT, BLOCK_FRAG);
    m_render_shaders["model_with_lighting"] = std::make_shared<RenderShader>(MODEL_WITH_LIGHTING_VERT, MODEL_WITH_LIGHTING_FRAG);
    m_render_shaders["light"]               = std::make_shared<RenderShader>(MODEL_VERT, LIGHT_FRAG);
}

void RenderPipeline::draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera) {
    bool use_ortho = false;

    // change the position of the spot lights (should be moved to the logic in the future)
    {
        float    time = static_cast<float>(glfwGetTime());
        uint32_t i    = 0;
        for (auto& [key, spot_light] : resource->getModifiableSpotLights()) {
            spot_light->setPosition(glm::vec3(sin(time * i), i, cos(time)));
            i += 1;
        }
    }

    // draw characters
    {
        auto shader = m_render_shaders["model_with_lighting"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(use_ortho));
        shader->setUniform("u_cam_pos", camera->getPosition());

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

        resource->getEntity("model")->draw(shader, resource);
    }

    // draw light mesh
    {
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
    {
        auto shader = m_render_shaders["block"];

        shader->use();
        shader->setUniform("u_time", static_cast<float>(glfwGetTime()));
        shader->setUniform("u_resolution", static_cast<float>(g_runtime_global_context.m_window_system->getWidth()),
                           static_cast<float>(g_runtime_global_context.m_window_system->getHeight()));
        shader->setUniform("u_view_projection", camera->getViewProjectionMatrix(use_ortho));

        resource->getEntity("minecraft_blocks")->draw(shader, resource);
    }
}

} // namespace BJTUGE
