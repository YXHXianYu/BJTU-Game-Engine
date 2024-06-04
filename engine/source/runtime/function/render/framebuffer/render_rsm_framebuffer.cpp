#include "runtime/function/render/framebuffer/render_shadow_framebuffer.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <cassert>
#include <functional>
#include <iostream>

namespace BJTUGE {

RenderRsmFramebuffer::RenderRsmFramebuffer(uint32_t width, uint32_t height) {
    m_width  = width;
    m_height = height;

    // frame buffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // position texture
    glGenTextures(1, &m_position_texture);
    glBindTexture(GL_TEXTURE_2D, m_position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_position_texture, 0);

    // normal texture
    glGenTextures(1, &m_normal_texture);
    glBindTexture(GL_TEXTURE_2D, m_normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_normal_texture, 0);

    // color (diffuse + specular) texture
    glGenTextures(1, &m_color_texture);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_color_texture, 0);

    // set which color attachment we'll use
    uint32_t attachments[3] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
    };
    glDrawBuffers(3, attachments);

    // depth texture
    glGenTextures(1, &m_depth_texture);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

    // check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "GBuffer Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderRsmFramebuffer::~RenderRsmFramebuffer() {
    // TODO
}

void RenderRsmFramebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderRsmFramebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderRsmFramebuffer::useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderRsmFramebuffer::usePositionTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_position_texture);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderRsmFramebuffer::useNormalTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_normal_texture);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderRsmFramebuffer::useColorTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderRsmFramebuffer::updateFramebufferSize(uint32_t width, uint32_t height) {
    m_width  = width;
    m_height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glBindTexture(GL_TEXTURE_2D, m_position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, m_normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace BJTUGE
