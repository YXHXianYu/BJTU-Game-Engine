#include "runtime/function/render/framebuffer/render_gbuffer_framebuffer.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_shader.h"
#include "runtime/function/window/window_system.h"

#include <glad/glad.h>

#include <cassert>
#include <functional>
#include <iostream>

namespace BJTUGE {

RenderGBufferFramebuffer::RenderGBufferFramebuffer(uint32_t width, uint32_t height) {
    m_width  = width;
    m_height = height;

    // frame buffer
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // position texture
    glGenTextures(1, &m_gbuffer_position);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gbuffer_position, 0);

    // normal texture
    glGenTextures(1, &m_gbuffer_normal);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gbuffer_normal, 0);

    // color (diffuse + specular) texture
    glGenTextures(1, &m_gbuffer_color);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gbuffer_color, 0);

    // transparent (color + reflection) texture
    glGenTextures(1, &m_gbuffer_transparent);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_transparent);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gbuffer_transparent, 0);

    // PBR (metallic, roughness, ao) texture
    glGenTextures(1, &m_gbuffer_pbr);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_pbr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_gbuffer_pbr, 0);

    // set which color attachment we'll use
    uint32_t attachments[5] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
    };
    glDrawBuffers(5, attachments);

    // depth texture
    glGenTextures(1, &m_depth_texture);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

    // check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "GBuffer Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // resize update
    g_runtime_global_context.m_window_system->registerOnResizeFunc(
        std::bind(&RenderGBufferFramebuffer::updateFramebufferSize, this, std::placeholders::_1, std::placeholders::_2));
}

RenderGBufferFramebuffer::~RenderGBufferFramebuffer() {
    // TODO
}

void RenderGBufferFramebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // alpha must be 0.0f
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderGBufferFramebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderGBufferFramebuffer::useGBufferPosition(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_position);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderGBufferFramebuffer::useGBufferNormal(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_normal);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderGBufferFramebuffer::useGBufferColor(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_color);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderGBufferFramebuffer::useGBufferTransparent(std::shared_ptr<RenderShader> shader, const std::string& name,
                                                     uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_transparent);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderGBufferFramebuffer::useGBufferPBR(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer_pbr);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderGBufferFramebuffer::useDepthTexture(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    shader->setUniform(name.c_str(), texture_id);
}

void RenderGBufferFramebuffer::updateFramebufferSize(uint32_t width, uint32_t height) {
    m_width  = width;
    m_height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glBindTexture(GL_TEXTURE_2D, m_gbuffer_position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, m_gbuffer_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, m_gbuffer_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, m_gbuffer_transparent);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, m_gbuffer_pbr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is not complete!" << std::endl;
        assert(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace BJTUGE
