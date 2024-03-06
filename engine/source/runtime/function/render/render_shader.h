#pragma once

#include <glad/glad.h>

#include <cstdint>
#include <optional>
#include <string>

namespace BJTUGE {

class RenderShader {

public:
    RenderShader(const char* vertex_shader, const char* fragment_shader);
    ~RenderShader();

    uint32_t get() const { return m_shader_program; }
    void     use() const { glUseProgram(m_shader_program); }

    /**
     * @brief Set a uniform value in the shader.
     *        Be careful! Before calling this function, the shader MUSE be in USE!!
     */
    void setUniform(const char* name, int value) const { glUniform1i(glGetUniformLocation(m_shader_program, name), value); }
    void setUniform(const char* name, bool value) const { glUniform1i(glGetUniformLocation(m_shader_program, name), value); }
    void setUniform(const char* name, float value) const { glUniform1f(glGetUniformLocation(m_shader_program, name), value); }

private:
    uint32_t m_shader_program;

    std::optional<std::string> getShaderCompileInfo(uint32_t shader);
    void                       assertShaderCompileSuccess(uint32_t shader);
    std::optional<std::string> getProgramCompileInfo(uint32_t program);
    void                       assertProgramCompileSuccess(uint32_t program);
};

} // namespace BJTUGE