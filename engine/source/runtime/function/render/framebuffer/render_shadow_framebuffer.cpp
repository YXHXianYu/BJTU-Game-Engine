#include "runtime/function/render/framebuffer/render_shadow_framebuffer.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <cassert>
#include <functional>
#include <iostream>

namespace BJTUGE {

RenderShadowFramebuffer::RenderShadowFramebuffer(uint32_t width, uint32_t height) {
    m_width  = width;
    m_height = height;

    float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};

    // frame buffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // depth texture
    glGenTextures(1, &m_depth_texture);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

    // color texture to store depth and depth^2
    glGenTextures(1, &m_color_texture);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, NULL);
    // set up MIPMAP
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glGenerateMipmap(GL_TEXTURE_2D); // No need. Because texture is empty now

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

    // check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Shadow Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderShadowFramebuffer::~RenderShadowFramebuffer() {
    glDeleteTextures(1, &m_depth_texture);
    glDeleteTextures(1, &m_color_texture);
    glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderShadowFramebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderShadowFramebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glGenerateMipmap(GL_TEXTURE_2D); // Because RenderShadowFramebuffer is generated every frame, so we can generate MIPMAP here!
}

void RenderShadowFramebuffer::useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_color_texture); // Use color texture to store depth and depth^2
    shader->setUniform(name.c_str(), texture_id);  // texture().rg => depth and depth^2
}

void RenderShadowFramebuffer::updateFramebufferSize(uint32_t width, uint32_t height) {
    m_width  = width;
    m_height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_width, m_height, 0, GL_RG, GL_FLOAT, NULL);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace BJTUGE
