#include "runtime/function/render/render_texture_3d.h"

#include "runtime/function/render/render_shader.h"

#include <cassert>
#include <cstring>
#include <iostream>

#include <stb_image.h>

namespace BJTUGE {

RenderTexture3D::RenderTexture3D(const std::vector<std::string>& picture_paths) {
    int is_need_to_add_bin = -1;

    m_depth = picture_paths.size();
    std::vector<unsigned char*> data(m_depth);

    for (uint32_t i = 0; i < m_depth; i++) {
        if (picture_paths[i] == "") {
            data[i] = nullptr;
        } else if (is_need_to_add_bin == -1) {
            data[i] = stbi_load(picture_paths[i].c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0);
            if (data[i] == nullptr) {
                data[i] = stbi_load(("./bin/" + picture_paths[i]).c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0);
                assert(data[i] != nullptr);
                is_need_to_add_bin = 1;
            } else {
                is_need_to_add_bin = 0;
            }
        } else {
            int width, height, channels;
            data[i] = stbi_load(((is_need_to_add_bin ? "./bin/" : "") + picture_paths[i]).c_str(), &width, &height, &channels, 0);
            assert(width == m_width && height == m_height && channels == m_channels);
        }
    }

    uint32_t       image_size = m_width * m_height * m_channels;
    unsigned char* data_3d    = new unsigned char[image_size * m_depth];

    for (uint32_t i = 0; i < m_depth; i++) {
        if (data[i] == nullptr) continue;
        memcpy(data_3d + (i * image_size), data[i], image_size);
        stbi_image_free(data[i]);
    }

    // gen texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_3D, m_texture);

    // configure
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    // 不使用MIPMAP
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 不使用MIPMAP
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 使用MIPMAP + 双线性插值（两个MIPMAP层之间不做插值, 取最近的）
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    // 使用MIPMAP + 三线性插值（两个MIPMAP层之间做插值）
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load data
    if (m_channels == 1) {
        std::cout << " - Loading an ONE channel texture, load it to RED channel." << std::endl;
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, m_width, m_height, m_depth, 0, GL_RED, GL_UNSIGNED_BYTE, data_3d);
    } else if (m_channels == 3) {
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, m_width, m_height, m_depth, 0, GL_RGB, GL_UNSIGNED_BYTE, data_3d);
    } else if (m_channels == 4) {
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, m_width, m_height, m_depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_3d);
    } else {
        std::cerr << "Unsupported texture channels: " << m_channels << std::endl;
        assert(false);
    }
    glGenerateMipmap(GL_TEXTURE_3D);

    // free resource
    stbi_image_free(data_3d);
}

RenderTexture3D::~RenderTexture3D() {
    glDeleteTextures(1, &m_texture);
}

void RenderTexture3D::use(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    // activate
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_3D, m_texture);
    // set the texture to the shader
    shader->setUniform(name.c_str(), texture_id);
}

} // namespace BJTUGE
