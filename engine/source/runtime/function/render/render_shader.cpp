#pragma once

#include "runtime/function/render/render_shader.h"

#include <glad/glad.h>

#include <cassert>
#include <iostream>

namespace BJTUGE {

RenderShader::RenderShader(const char* vertex_shader_code, const char* fragment_shader_code) {
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    assertShaderCompileSuccess(vertex_shader);

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    assertShaderCompileSuccess(fragment_shader);

    m_shader_program = glCreateProgram();
    glAttachShader(m_shader_program, vertex_shader);
    glAttachShader(m_shader_program, fragment_shader);
    glLinkProgram(m_shader_program);
    assertProgramCompileSuccess(m_shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

RenderShader::~RenderShader() {
    glDeleteProgram(m_shader_program);
}

void RenderShader::setTexture(const char* name, uint32_t texture_id, std::shared_ptr<RenderTexture> texture) {
    texture->use(texture_id);
    setUniform(name, texture_id);
}

std::optional<std::string> RenderShader::getShaderCompileInfo(uint32_t shader) {
    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success) {
        return {};
    } else {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        return std::string(info_log);
    }
}

void RenderShader::assertShaderCompileSuccess(uint32_t shader) {
    auto info = getShaderCompileInfo(shader);
    if (info) {
        std::cerr << "Shader compile error: " << *info << std::endl;
        assert(false);
    }
}

std::optional<std::string> RenderShader::getProgramCompileInfo(uint32_t program) {
    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success) {
        return {};
    } else {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        return std::string(info_log);
    }
}

void RenderShader::assertProgramCompileSuccess(uint32_t program) {
    auto info = getProgramCompileInfo(program);
    if (info) {
        std::cerr << "Program compile error: " << *info << std::endl;
        assert(false);
    }
}

} // namespace BJTUGE