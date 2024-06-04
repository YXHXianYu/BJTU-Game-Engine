#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace BJTUGE {

class RenderShader;

class RenderRsmFramebuffer {

public:
    RenderRsmFramebuffer(uint32_t width, uint32_t height);
    ~RenderRsmFramebuffer();

    void bind() const;
    void unbind() const;

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

    void useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;
    void usePositionTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;
    void useNormalTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;
    void useColorTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;

    void updateFramebufferSize(uint32_t width, uint32_t height);

private:
    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_framebuffer;
    uint32_t m_depth_texture;
    uint32_t m_position_texture;
    uint32_t m_normal_texture;
    uint32_t m_color_texture;
};

}
