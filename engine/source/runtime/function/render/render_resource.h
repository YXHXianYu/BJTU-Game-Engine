#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace BJTUGE {

class RenderMesh;
class RenderEntity;
class RenderTextureBase;

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
     * @brief Get a entity by its key
     */
    std::shared_ptr<RenderEntity> getEntity(const std::string& key) const { return m_render_entities.at(key); }

    /**
     * @brief Load a RenderEntity from a file.
     */
    std::shared_ptr<RenderEntity> loadEntityFromFile(const std::string& file_path);

private:
    std::unordered_map<std::string, std::shared_ptr<RenderEntity>>      m_render_entities;
    std::unordered_map<std::string, std::shared_ptr<RenderTextureBase>> m_render_textures;

private:
    void loadMinecraft3DTexture();
};

} // namespace BJTUGE