#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

class RenderMesh;
class RenderEntity;
class RenderTextureBase;
class RenderMeshBase;

class RenderResource {

public:
    void initialize();
    void clear();

    /**
     * @brief 1. Load a texture from a file.
     *        2. Add a texture to the resource manager.
     */
    void addTexture(const std::string& key, std::shared_ptr<RenderTextureBase> texture);

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
     * @brief Load a RenderEntity from a file.
     */
    std::shared_ptr<RenderEntity> loadEntityFromFile(const std::string& file_path);

private:
    std::unordered_map<std::string, std::shared_ptr<RenderEntity>>      m_render_entities;
    std::unordered_map<std::string, std::shared_ptr<RenderTextureBase>> m_render_textures;

private:
    std::shared_ptr<RenderTextureBase> loadMinecraftTexture();

    std::shared_ptr<RenderMeshBase> loadCubeMesh();

    std::shared_ptr<RenderEntity> loadMinecraftBlocks();
    std::shared_ptr<RenderEntity> loadCube();
    std::shared_ptr<RenderEntity> loadCharacters();
    std::shared_ptr<RenderEntity> loadPlainBlocks();
};

} // namespace BJTUGE