#include "runtime/function/render/render_texture.h"

#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace BJTUGE {

RenderTexture::RenderTexture(const std::string& picture_path) {

    static bool is_first_time = true;
    if (is_first_time) {
        is_first_time = false;
        stbi_set_flip_vertically_on_load(true);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    unsigned char* data = stbi_load(picture_path.c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0);
    if (data == nullptr) { data = stbi_load(("./bin/" + picture_path).c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0); }
    assert(data);

    std::cout << "Load texture \"" << picture_path << "\": " << m_width << "x" << m_height << "x" << m_channels << std::endl;

    // gen texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // configure
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 不使用MIPMAP
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 使用MIPMAP + 双线性插值（两个MIPMAP层之间不做插值, 取最近的）
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    // 使用MIPMAP + 三线性插值（两个MIPMAP层之间做插值）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load data
    if (m_channels == 1) {
        std::cout << " - Loading an ONE channel texture, load it to RED channel." << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    } else if (m_channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (m_channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        std::cerr << "Unsupported texture channels: " << m_channels << std::endl;
        assert(false);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    // free resource
    stbi_image_free(data);
}

RenderTexture::~RenderTexture() {
    glDeleteTextures(1, &m_texture);
}

} // namespace BJTUGE