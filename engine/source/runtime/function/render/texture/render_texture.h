#pragma once

#include "runtime/function/render/texture/render_texture_base.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include <glad/glad.h>

#include <assimp/scene.h>

namespace BJTUGE {

class RenderShader;

enum class TextureType {
    DEFAULT,
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
};

class RenderTexture : public RenderTextureBase {

public:
    RenderTexture(const std::string& picture_path, const std::string& type = "default", const std::string& sample_method = "nearest");
    RenderTexture(const aiTexture* texture, const std::string& type = "default", const std::string& sample_method = "nearest");
    virtual ~RenderTexture();
    RenderTexture(const RenderTexture&)            = delete;
    RenderTexture& operator=(const RenderTexture&) = delete;

    virtual void use(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const override;

    std::string getType() const { return TEXTURE_TYPE_TO_STRING.at(m_type); }

private:
    void generateTexture(unsigned char* data, const std::string& sample_method);

public:
    static const std::unordered_map<TextureType, std::string> TEXTURE_TYPE_TO_STRING;
    static const std::unordered_map<std::string, TextureType> STRING_TO_TEXTURE_TYPE;

private:
    uint32_t    m_texture;
    TextureType m_type;
    uint32_t    m_width;
    uint32_t    m_height;
    uint32_t    m_channels;

    const std::string m_sample_method;
};

} // namespace BJTUGE