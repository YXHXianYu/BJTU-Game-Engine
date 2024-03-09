#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

#include <glad/glad.h>

#include <assimp/scene.h>

namespace BJTUGE {

class RenderShader;

enum class TextureType {
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
};

class RenderTexture {

public:
    RenderTexture(const std::string& picture_path, const std::string& type);
    RenderTexture(const aiTexture* texture, const std::string& type);
    ~RenderTexture();
    RenderTexture(const RenderTexture&)            = delete;
    RenderTexture& operator=(const RenderTexture&) = delete;

    void use(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const;

    std::string getType() const { return TEXTURE_TYPE_TO_STRING.at(m_type); }

private:
    void generateTexture(unsigned char* data);

public:
    static const std::unordered_map<TextureType, std::string> TEXTURE_TYPE_TO_STRING;
    static const std::unordered_map<std::string, TextureType> STRING_TO_TEXTURE_TYPE;

private:
    uint32_t    m_texture;
    TextureType m_type;
    uint32_t    m_width;
    uint32_t    m_height;
    uint32_t    m_channels;
};

} // namespace BJTUGE