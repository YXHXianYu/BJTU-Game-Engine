#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace BJTUGE {

class RenderMesh;
class RenderEntity;
class RenderTextureBase;
class RenderMeshBase;
class RenderSpotLight;
class RenderDirectionLight;
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
     * @brief Get a modifiable RenderDirectionLights
     */
    const glm::mat4 getLightSpaceMatrix() const { return m_light_space_matrix; }

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

    void bindKeyboardEvent();
    void onKey(int key, int scancode, int action, int mods);

    void updateLightSpaceMatrix();

private:
    std::shared_ptr<RenderTextureBase> loadMinecraftTexture();
    std::shared_ptr<RenderTextureBase> loadNoiseTexture();
    std::shared_ptr<RenderTextureBase> loadFractalNoiseTexture();
    std::shared_ptr<RenderMeshBase>    loadCubeMesh();
    std::shared_ptr<RenderMeshBase>    loadSquareMesh();

    void loadLightingCubeToResource(); // TODO: 统一格式

    std::shared_ptr<RenderEntity> loadMinecraftBlocks();
    std::shared_ptr<RenderEntity> loadCharacters();
    std::shared_ptr<RenderEntity> loadPlainBlocks();
    std::shared_ptr<RenderEntity> loadWater();
    std::shared_ptr<RenderEntity> loadPostprocessRectangle();

    // assignments
    std::shared_ptr<RenderEntity> loadAssignments();
    std::shared_ptr<RenderEntity> load2DShape();
    std::shared_ptr<RenderEntity> loadSquareLovekdl();
    std::shared_ptr<RenderEntity> loadCubesFJQ();
    std::shared_ptr<RenderEntity> loadCatsCJX();
    // cjx
    std::shared_ptr<RenderEntity> loadCreeper();
    std::shared_ptr<RenderEntity> loadHouse();
    std::shared_ptr<RenderEntity> loadTree();
    std::shared_ptr<RenderEntity> loadSheep();
    std::shared_ptr<RenderEntity> loadSword();

    glm::vec3 m_sun_light_direction{-1.0 / sqrt(5), -2.0 / sqrt(5), -1.0 / sqrt(5)};
    float     m_sun_light_intensity{1.0f};
    glm::mat4 m_light_space_matrix; // update by constructor in initialization; update by onKey() while gaming
};

} // namespace BJTUGE
