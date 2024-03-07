#pragma once

#include <cstdint>
#include <string>

#include <glad/glad.h>

namespace BJTUGE {

class RenderTexture {

public:
    RenderTexture(const std::string& picture_path, uint32_t texture_id);
    ~RenderTexture();
    RenderTexture(const RenderTexture&)            = delete;
    RenderTexture& operator=(const RenderTexture&) = delete;

    uint32_t get() const { return m_texture; }
    uint32_t getId() const { return m_texture_id; }
    void     use() const {
        glActiveTexture(GL_TEXTURE0 + m_texture_id);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

private:
    uint32_t m_texture_id;
    uint32_t m_texture;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_channels;
};

} // namespace BJTUGE