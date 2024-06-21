#pragma once

#include "runtime/function/render/mesh/render_mesh_base.h"
#include "runtime/function/render/render_material.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace BJTUGE {

class RenderTexture;

class RenderMesh : public RenderMeshBase {

public:
    RenderMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<std::string>& textures = {});
    virtual ~RenderMesh() override;
    RenderMesh(const RenderMesh&)            = delete;
    RenderMesh& operator=(const RenderMesh&) = delete;

    /**
     * @brief Add texture to the mesh and automatically register the texture to the resource manager
     * @param path     The path of the texture
     * @param texture  The texture
     * @param resource The resource manager
     */
    void addTexture(const std::string& path, std::shared_ptr<RenderTexture> texture, std::shared_ptr<RenderResource> resource);

    void resetTexture() { m_textures.clear(); }

    virtual void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) override;

    void setModelMatrix(const glm::mat4& model) { m_model = model; }

    void setDiffuseColor(const glm::vec3& color) { m_diffuse_color = color; }

    void output() const;

private:
    std::vector<Vertex>      m_vertices;
    std::vector<uint32_t>    m_indices;
    std::vector<std::string> m_textures;
    glm::vec3                m_diffuse_color{1.0f};
    glm::mat4                m_model{1.0f};

    uint32_t m_vao{0};
    uint32_t m_vbo{0};
    uint32_t m_ebo{0};

    RenderMaterial m_material;
};

} // namespace BJTUGE
