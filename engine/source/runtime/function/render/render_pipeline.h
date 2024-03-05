#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <string>

namespace BJTUGE {

class RenderPipeline {

public:
    void initialize();

    uint32_t getShaderProgram() const { return m_shader_program; }

private:
    // std::vector<uint8_t> m_vertex_shader;
    // std::vector<uint8_t> m_fragment_shader;

    uint32_t m_shader_program;

    std::optional<std::string> getShaderCompileInfo(uint32_t shader);
    void assertShaderCompileSuccess(uint32_t shader);

    std::optional<std::string> getProgramCompileInfo(uint32_t program);
    void assertProgramCompileSuccess(uint32_t program);

};

}
