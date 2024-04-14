#pragma once

#include "runtime/function/render/lighting/render_direction_light.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace BJTUGE {

class RenderMesh;
class RenderEntity;
class RenderSpotLight;
class RenderTextureBase;
class RenderMeshBase;
class RenderMinecraftBlocksManager;

class RenderResource {

public:
    void initialize();
    void clear();

    /**
     * @brief Register texture to the resource manager
     *        This method will be invoked by RenderMesh automatically.
     *        Most of the time, you don't need to call this method manually.
     */
    void registerTexture(const std::string& key, std::shared_ptr<RenderTextureBase> texture);

    /**
     * @brief Get a texture by its key
     */
    std::shared_ptr<RenderTextureBase> getTexture(const std::string& key) const;

    /**
     * @brief Whether the resource manager has a texture
     */
    bool hasTexture(const std::string& key) const { return m_render_textures.find(key) != m_render_textures.end(); }

    /**
     * @brief Get a entity by its key
     */
    std::shared_ptr<RenderEntity> getEntity(const std::string& key) const;

    /**
     * @brief Whether the resource manager has a texture
     */
    bool hasEntity(const std::string& key) const { return m_render_entities.find(key) != m_render_entities.end(); }

    /**
     * @brief Add a RenderEntity to the resource manager
     */
    void addEntity(const std::string& key, std::shared_ptr<RenderEntity> entity) { m_render_entities[key] = entity; }

    /**
     * @brief Get a RenderSpotLight by its key
     */
    std::shared_ptr<RenderSpotLight> getSpotLight(const std::string& key) const { return m_spot_lights.at(key); }

    /**
     * @brief Get a RenderSpotLights in unordered map format
     */
    const std::unordered_map<std::string, std::shared_ptr<RenderSpotLight>>& getSpotLights() const { return m_spot_lights; }

    /**
     * @brief Get a modifiable RenderSpotLights in unordered map format
     *        Use this method carefully!
     *        (This method will be deprecated in the future.)
     */
    std::unordered_map<std::string, std::shared_ptr<RenderSpotLight>> getModifiableSpotLights() const { return m_spot_lights; }

    /**
     * @brief Get a RenderDirectionLights in unordered map format
     */
    const std::unordered_map<std::string, std::shared_ptr<RenderDirectionLight>>& getDirectionLights() const { return m_direction_lights; }

    /**
     * @brief Load a RenderEntity from a file.
     */
    std::shared_ptr<RenderEntity> loadEntityFromFile(const std::string& file_path);

    /**
     * @brief Get the RenderMinecraftBlocksManager to add or remove blocks.
     *        `startTransfer` and `endTransfer` must be called before and after adding or removing blocks.
     */
    std::shared_ptr<RenderMinecraftBlocksManager> getRenderMinecraftBlocksManager() { return m_render_minecraft_blocks_manager; }

private:
    std::unordered_map<std::string, std::shared_ptr<RenderSpotLight>>      m_spot_lights;
    std::unordered_map<std::string, std::shared_ptr<RenderDirectionLight>> m_direction_lights;
    std::unordered_map<std::string, std::shared_ptr<RenderEntity>>         m_render_entities;
    std::unordered_map<std::string, std::shared_ptr<RenderTextureBase>>    m_render_textures;

    std::shared_ptr<RenderMinecraftBlocksManager> m_render_minecraft_blocks_manager{nullptr};

private:
    std::shared_ptr<RenderTextureBase> loadMinecraftTexture();
    std::shared_ptr<RenderMeshBase>    loadCubeMesh();

    void loadLightingCubeToResource(); // TODO: 统一格式

    std::shared_ptr<RenderEntity> loadMinecraftBlocks();
    std::shared_ptr<RenderEntity> loadCharacters();
    std::shared_ptr<RenderEntity> loadPlainBlocks();
    std::shared_ptr<RenderEntity> loadCubes();
};

} // namespace BJTUGE