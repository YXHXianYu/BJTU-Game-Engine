#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace BJTUGE {

class RenderMesh;
class RenderEntity;
class RenderTexture;
class SpotLight;

class RenderResource {

public:
    void initialize();
    void clear();

    /**
     * @brief 1. Load a texture from a file.
     *        2. Add a texture to the resource manager.
     */
    void addTexture(const std::string& key, std::shared_ptr<RenderTexture> texture);

    /**
     * @brief Get a texture by its key
     */
    std::shared_ptr<RenderTexture> getTexture(const std::string& key) const { return m_render_textures.at(key); }

    /**
     * @brief Get a entity by its key
     */
    std::shared_ptr<RenderEntity> getEntity(const std::string& key) const { return m_render_entities.at(key); }

    /**
     * @brief Get a SpotLight by its key
     */
    std::shared_ptr<SpotLight> getSpotLight(const std::string& key) const { return m_spot_lights.at(key); }

    /**
     * @brief Get a SpotLight by its key
     */
    std::vector<std::shared_ptr<SpotLight>> getSpotLights() const {
        // TODO: better way?
        std::vector<std::shared_ptr<SpotLight>> spot_lights;
        for (const auto& [_key, value] : m_spot_lights) {
            spot_lights.push_back(value);
        }
        return spot_lights;
    }

    /**
     * @brief Load a RenderEntity from a file.
     */
    std::shared_ptr<RenderEntity> loadEntityFromFile(const std::string& file_path);

private:
    std::unordered_map<std::string, std::shared_ptr<RenderEntity>>  m_render_entities;
    std::unordered_map<std::string, std::shared_ptr<SpotLight>>     m_spot_lights;
    std::unordered_map<std::string, std::shared_ptr<RenderTexture>> m_render_textures;
};

} // namespace BJTUGE