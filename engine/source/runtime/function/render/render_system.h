#pragma once

#include <memory>

namespace BJTUGE {

class RenderResource;
class RenderPipeline;
class RenderCamera;
class RenderMinecraftBlocksManager;

class RenderSystem {

public:
    void initialize();
    void tick(float delta_time);
    void clear();

private:
    std::shared_ptr<RenderResource>               m_render_resource{nullptr};
    std::shared_ptr<RenderPipeline>               m_render_pipeline{nullptr};
    std::shared_ptr<RenderCamera>                 m_render_camera{nullptr};
    std::shared_ptr<RenderMinecraftBlocksManager> m_render_minecraft_blocks_manager{nullptr};
};

} // namespace BJTUGE
