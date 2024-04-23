#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace BJTUGE {

class RenderShader;

class RenderFramebuffer {

public:
    RenderFramebuffer(uint32_t width, uint32_t height);
    ~RenderFramebuffer();

    void bind() const;
    void unbind() const;
    void useColorTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;
    void useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;

    void updateFramebufferSize(uint32_t width, uint32_t height);

private:
    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_framebuffer;
    uint32_t m_color_texture;
    uint32_t m_depth_texture;
};

} // namespace BJTUGE