#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace BJTUGE {

class RenderShader;

class RenderShadowFramebuffer {

public:
    RenderShadowFramebuffer(uint32_t width, uint32_t height);
    ~RenderShadowFramebuffer();

    void bind() const;
    void unbind() const;
    void useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;

    void updateFramebufferSize(uint32_t width, uint32_t height);

private:
    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_framebuffer;
    uint32_t m_depth_texture;
};

}
