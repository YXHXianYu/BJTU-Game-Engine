#pragma once

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
class RenderGBufferFramebuffer;

class RenderPipeline {

public:
    void initialize();

    void draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

    void tick(uint32_t GameCommand, std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

private:
    void draw_gbuffer(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);
    void draw_shading(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);
    void draw_shadow_map(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);
    void draw_postprocess(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

    glm::mat4 getLightSpaceMatrix();

    std::shared_ptr<RenderShader>&      getShader(const char* name);
    std::shared_ptr<RenderFramebuffer>& getFramebuffer(const char* name);

    void bindKeyboardEvent();
    void onKey(int key, int scancode, int action, int mods);

private:
    std::unordered_map<std::string, std::shared_ptr<RenderShader>>      m_render_shaders;
    std::unordered_map<std::string, std::shared_ptr<RenderFramebuffer>> m_render_framebuffers;
    std::shared_ptr<RenderShadowFramebuffer>                            m_shadow_framebuffer;
    std::shared_ptr<RenderGBufferFramebuffer>                           m_gbuffer_framebuffer;

    uint32_t  m_shadow_map_width{2048 * 4};
    uint32_t  m_shadow_map_height{2048 * 4};
    glm::mat4 m_light_space_matrix;

    bool m_render_block{true};
    bool m_render_character{true};
    bool m_render_light{true};
    bool m_render_transparent{true};
    bool m_use_ortho{true};

    bool m_render_assignments{true};

    bool m_render_by_depth{false};
    bool m_is_enable_shadow_map{true};

    uint32_t  m_water_mode{3};
    uint32_t  m_fxaa_mode{2};
    float     m_cloud_thickness{0.6};
    glm::vec3 m_sky_color{0.47, 0.66, 1.00};
};

} // namespace BJTUGE
