#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace BJTUGE {

class RenderShader;

class RenderGBufferFramebuffer {

public:
    RenderGBufferFramebuffer(uint32_t width, uint32_t height);
    ~RenderGBufferFramebuffer();

    void bind() const;
    void unbind() const;
    void useGBufferPosition(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;
    void useGBufferNormal(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;
    void useGBufferColor(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;
    void useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;

    void updateFramebufferSize(uint32_t width, uint32_t height);

private:
    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_framebuffer;
    uint32_t m_gbuffer_position;
    uint32_t m_gbuffer_normal;
    uint32_t m_gbuffer_color; // diffuse + specular
    uint32_t m_depth_texture;
};

} // namespace BJTUGE
