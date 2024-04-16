#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <glm/glm.hpp>

namespace BJTUGE {

class RenderResource;
class RenderMeshBase;
class RenderShader;

class RenderEntity {

public:
    RenderEntity() = default;
    RenderEntity(const std::unordered_map<std::string, std::shared_ptr<RenderMeshBase>>& render_meshes) : m_render_meshes(render_meshes){};

    // mesh
    void addMesh(const std::string& key, const std::shared_ptr<RenderMeshBase>& render_mesh) { m_render_meshes[key] = render_mesh; }
    std::shared_ptr<RenderMeshBase> getMesh(const std::string& key) const {
        assert(hasMesh(key));
        return m_render_meshes.at(key);
    }
    bool hasMesh(const std::string& key) const { return m_render_meshes.find(key) != m_render_meshes.end(); }

    // entity
    void addEntity(const std::string& key, std::shared_ptr<RenderEntity> render_entity) { m_render_entities[key] = render_entity; }
    void setEntity(const std::string& key, std::shared_ptr<RenderEntity> render_entity) { m_render_entities[key] = render_entity; }
    std::shared_ptr<RenderEntity> getEntity(const std::string& key) const {
        assert(hasEntity(key));
        return m_render_entities.at(key);
    }
    bool hasEntity(const std::string& key) const { return m_render_entities.find(key) != m_render_entities.end(); }
    void removeEntity(const std::string& key) { m_render_entities.erase(key); }

    std::shared_ptr<RenderEntity> get(const std::string& key) const { return m_render_entities.at(key); }

    // model matrix
    void      setModelMatrix(const glm::mat4& model) { m_model = model; }
    glm::mat4 getModelMatrix() const { return m_model; }

    // draw
    void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model = glm::mat4(1.0f));

    // output
    void output(uint32_t level = 0) const;

private:
    std::unordered_map<std::string, std::shared_ptr<RenderMeshBase>> m_render_meshes;
    std::unordered_map<std::string, std::shared_ptr<RenderEntity>>   m_render_entities;

    glm::mat4 m_model{1.0f};
};

} // namespace BJTUGE