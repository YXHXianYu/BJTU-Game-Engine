#include "runtime/function/render/render_texture.h"

#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace BJTUGE {

RenderTexture::RenderTexture(const std::string& picture_path, uint32_t texture_id) {

    static bool is_first_time = true;
    if (is_first_time) {
        is_first_time = false;
        stbi_set_flip_vertically_on_load(true);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    unsigned char* data = stbi_load(picture_path.c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0);
    if (data == nullptr) { data = stbi_load(("./bin/" + picture_path).c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0); }
    assert(data);

    std::cout << picture_path << ": " << m_width << "x" << m_height << "x" << m_channels << std::endl;

    // bind texture unit
    assert(texture_id <= 15);
    m_texture_id = texture_id;

    // gen texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // configure
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load data
    if (m_channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (m_channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    // free resource
    stbi_image_free(data);
}

RenderTexture::~RenderTexture() {
    glDeleteTextures(1, &m_texture);
}

} // namespace BJTUGE