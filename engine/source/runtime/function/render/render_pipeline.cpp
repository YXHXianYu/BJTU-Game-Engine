#include "runtime/function/render/render_pipeline.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <iostream>

#include <basic_vert.h>
#include <basic_frag.h>

namespace BJTUGE {

void RenderPipeline::initialize() {
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &BASIC_VERT, NULL);
    glCompileShader(vertex_shader);
    assertShaderCompileSuccess(vertex_shader);

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &BASIC_FRAG, NULL);
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

std::optional<std::string> RenderPipeline::getShaderCompileInfo(uint32_t shader) {
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

void RenderPipeline::assertShaderCompileSuccess(uint32_t shader) {

    assert(!getShaderCompileInfo(shader));
}

std::optional<std::string> RenderPipeline::getProgramCompileInfo(uint32_t program) {
    int32_t success;
    glGetProgramiv(program, GL_COMPILE_STATUS, &success);

    if (success) {
        return {};
    } else {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        return std::string(info_log);
    }
}

void RenderPipeline::assertProgramCompileSuccess(uint32_t program) {
    assert(!getProgramCompileInfo(program));
}

} // namespace BJTUGE
