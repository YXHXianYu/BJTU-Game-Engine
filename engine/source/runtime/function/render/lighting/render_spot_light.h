#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

class RenderResource;
class RenderMeshBase;
class RenderShader;

class RenderSpotLight {

public:
    RenderSpotLight() = default;
    RenderSpotLight(const std::unordered_map<std::string, std::shared_ptr<RenderMeshBase>>& render_meshes) : m_render_meshes(render_meshes){};

    // mesh
    void addMesh(const std::string& key, const std::shared_ptr<RenderMeshBase>& render_mesh) { m_render_meshes[key] = render_mesh; }
    std::shared_ptr<RenderMeshBase> getMesh(const std::string& key) const { return m_render_meshes.at(key); }
    bool                            hasMesh(const std::string& key) const { return m_render_meshes.find(key) != m_render_meshes.end(); }

    // position
    void setPosition(const glm::vec3& position) {
        m_position = position;
        m_model    = glm::translate(glm::mat4(1.0f), position);
    }
    glm::vec3 getPosition() const { return m_position; }

    // color
    void      setColor(const glm::vec3& color) { m_color = color; }
    glm::vec3 getColor() const { return m_color; }

    // model matrix
    // to make sure the `m_position` is correspond to the m_model, use `setPosition` instead
    // void      setModelMatrix(const glm::mat4& model) { m_model = model; }
    // glm::mat4 getModelMatrix() const { return m_model; }

    // draw
    void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model = glm::mat4(1.0f));

private:
    std::unordered_map<std::string, std::shared_ptr<RenderMeshBase>> m_render_meshes;
    glm::vec3                                                        m_position{0.0f};
    glm::vec3                                                        m_color{1.0f};

    glm::mat4 m_model{1.0f};
};

} // namespace BJTUGE