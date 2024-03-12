#include "runtime/function/render/render_texture.h"

#include "runtime/function/render/render_shader.h"

#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace BJTUGE {

const std::unordered_map<TextureType, std::string> RenderTexture::TEXTURE_TYPE_TO_STRING = {
    {TextureType::DIFFUSE, "diffuse"},
    {TextureType::SPECULAR, "specular"},
    {TextureType::NORMAL, "normal"},
    {TextureType::HEIGHT, "height"},
};

const std::unordered_map<std::string, TextureType> RenderTexture::STRING_TO_TEXTURE_TYPE = {
    {"diffuse", TextureType::DIFFUSE},
    {"specular", TextureType::SPECULAR},
    {"normal", TextureType::NORMAL},
    {"height", TextureType::HEIGHT},
};

static bool is_first_time = true;

void first_time_initialize() {
    if (!is_first_time) { return; }
    is_first_time = false;
    stbi_set_flip_vertically_on_load(true);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

RenderTexture::RenderTexture(const std::string& picture_path, const std::string& type) : m_type(STRING_TO_TEXTURE_TYPE.at(type)) {
    first_time_initialize();

    unsigned char* data = stbi_load(picture_path.c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0);
    if (data == nullptr) { data = stbi_load(("./bin/" + picture_path).c_str(), (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0); }
    if (data == nullptr) {
        std::cerr << "Failed to load texture: " << picture_path << std::endl;
        assert(data);
    }

    // std::cout << "\tLoad texture \"" << picture_path << "\": " << m_width << "x" << m_height << "x" << m_channels << std::endl;

    generateTexture(data);
}

RenderTexture::RenderTexture(const aiTexture* texture, const std::string& type) {
    first_time_initialize();

    unsigned char* data;
    auto           pcData = reinterpret_cast<const unsigned char*>(texture->pcData);
    if (texture->mHeight == 0) {
        data = stbi_load_from_memory(pcData, texture->mWidth, (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0);
    } else {
        data = stbi_load_from_memory(pcData, texture->mWidth * texture->mHeight, (int*)&m_width, (int*)&m_height, (int*)&m_channels, 0);
    }

    std::cout << "\tLoad texture from memory:" << m_width << "x" << m_height << "x" << m_channels << std::endl;

    generateTexture(data);
}

RenderTexture::~RenderTexture() {
    glDeleteTextures(1, &m_texture);
}

void RenderTexture::use(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const {
    // activate
    glActiveTexture(GL_TEXTURE0 + texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    // set the texture to the shader
    shader->setUniform(name.c_str(), texture_id);
}

void RenderTexture::generateTexture(unsigned char* data) {
    // gen texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // configure
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 不使用MIPMAP
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 不使用MIPMAP
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 使用MIPMAP + 双线性插值（两个MIPMAP层之间不做插值, 取最近的）
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

    // 使用MIPMAP + 三线性插值（两个MIPMAP层之间做插值）
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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

} // namespace BJTUGE