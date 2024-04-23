#pragma once

#include "runtime/function/render/texture/render_texture_base.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>

namespace BJTUGE {

class RenderShader;

class RenderTexture3D : public RenderTextureBase {

public:
    RenderTexture3D(const std::vector<std::string>& picture_paths);
    virtual ~RenderTexture3D() override;
    RenderTexture3D(const RenderTexture3D&)            = delete;
    RenderTexture3D& operator=(const RenderTexture3D&) = delete;

    virtual void use(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const override;

private:
    uint32_t m_texture;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_depth;
    uint32_t m_channels;
};

} // namespace BJTUGE