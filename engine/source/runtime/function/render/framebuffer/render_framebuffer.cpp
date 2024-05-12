#include "runtime/function/render/framebuffer/render_framebuffer.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <cassert>
#include <iostream>

namespace BJTUGE {

RenderFramebuffer::RenderFramebuffer(uint32_t width, uint32_t height, bool enable_linear_filter) {
    m_width  = width;
    m_height = height;

    // frame buffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // color texture
    glGenTextures(1, &m_color_texture);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    if (enable_linear_filter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

    // depth texture
    glGenTextures(1, &m_depth_texture);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

    // render buffer
    // glGenRenderbuffers(1, &m_renderbuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer);

    // check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // bind resize callback
    g_runtime_global_context.m_window_system->registerOnResizeFunc(
        std::bind(&RenderFramebuffer::updateFramebufferSize, this, std::placeholders::_1, std::placeholders::_2));
}

RenderFramebuffer::~RenderFramebuffer() {
    // TODO
}

void RenderFramebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderFramebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderFramebuffer::useColorTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderFramebuffer::useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderFramebuffer::updateFramebufferSize(uint32_t width, uint32_t height) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glBindTexture(GL_TEXTURE_2D, m_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace BJTUGE
