#pragma once

#include "runtime/function/input/input_system.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

class RenderResource;
class RenderCamera;
class RenderShader;
class RenderFramebuffer;
class RenderShadowFramebuffer;

class RenderPipeline {

public:
    void initialize();

    void draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

    void draw_shadow_map(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

    void tick(uint32_t GameCommand, std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

    glm::mat4 getLightSpaceMatrix();

private:
    std::unordered_map<std::string, std::shared_ptr<RenderShader>> m_render_shaders;

    std::unordered_map<std::string, std::shared_ptr<RenderFramebuffer>> m_render_framebuffers;

    std::shared_ptr<RenderShadowFramebuffer> m_render_shadow_framebuffer;

    uint32_t m_shadow_map_width{2048 * 4};
    uint32_t m_shadow_map_height{2048 * 4};
    glm::mat4 m_light_space_matrix;

    bool render_block{true};
    bool render_character{true};
    bool render_light{true};
    bool use_ortho{true};

    bool render_assignments{true};

    bool render_by_depth{false};
};

} // namespace BJTUGE
