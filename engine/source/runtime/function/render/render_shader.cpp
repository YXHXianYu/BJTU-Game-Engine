#include "runtime/function/render/render_shader.h"

#include <glad/glad.h>

#include <cassert>
#include <iostream>

namespace BJTUGE {

RenderShader::RenderShader(const char* vertex_shader_code, const char* fragment_shader_code, const std::string& shader_name) {
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    assertShaderCompileSuccess(vertex_shader, shader_name + "(Vertex Shader)");

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    assertShaderCompileSuccess(fragment_shader, shader_name + "(Fragment Shader)");

    m_shader_program = glCreateProgram();
    glAttachShader(m_shader_program, vertex_shader);
    glAttachShader(m_shader_program, fragment_shader);
    glLinkProgram(m_shader_program);
    assertProgramCompileSuccess(m_shader_program, shader_name + "(Link Program)");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

RenderShader::~RenderShader() {
    glDeleteProgram(m_shader_program);
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

void RenderShader::assertShaderCompileSuccess(uint32_t shader, const std::string& shader_name) {
    auto info = getShaderCompileInfo(shader);
    if (info) {
        std::cerr << "Shader compile error: " << shader_name << ": " << *info << std::endl;
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

void RenderShader::assertProgramCompileSuccess(uint32_t program, const std::string& shader_name) {
    auto info = getProgramCompileInfo(program);
    if (info) {
        std::cerr << "Program compile error in " << shader_name << ": " << *info << std::endl;
        assert(false);
    }
}

} // namespace BJTUGE
