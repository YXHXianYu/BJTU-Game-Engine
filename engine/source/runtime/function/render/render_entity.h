#pragma once

#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_texture.h"

#include <vector>
#include <memory>

#include <glm/glm.hpp>

namespace BJTUGE {

class RenderEntity {

public:
    RenderEntity() = default;

    void addRenderMesh(const std::shared_ptr<RenderMesh>& render_mesh) { m_render_meshes.push_back(render_mesh); }
    void addRenderTexture(const std::shared_ptr<RenderTexture>& render_texture) { m_render_textures.push_back(render_texture); }

    const std::vector<std::shared_ptr<RenderMesh>>&    getRenderMeshes() const { return m_render_meshes; }
    const std::vector<std::shared_ptr<RenderTexture>>& getRenderTextures() const { return m_render_textures; }

    void      setModelMatrix(const glm::mat4& model) { m_model = model; }
    glm::mat4 getModelMatrix() const { return m_model; }

private:
    std::vector<std::shared_ptr<RenderMesh>>    m_render_meshes;
    std::vector<std::shared_ptr<RenderTexture>> m_render_textures;

    glm::mat4 m_model{1.0f};
};

} // namespace BJTUGE