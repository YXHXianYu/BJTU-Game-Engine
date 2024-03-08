#pragma once

#include <memory>

namespace BJTUGE {

class RenderResource;
class RenderPipeline;
class RenderCamera;

class RenderSystem {

public:
    void initialize();
    void tick(float delta_time);
    void clear();

private:
    std::shared_ptr<RenderResource> m_render_resource{nullptr};
    std::shared_ptr<RenderPipeline> m_render_pipeline{nullptr};
    std::shared_ptr<RenderCamera>   m_render_camera{nullptr};
};

} // namespace BJTUGE
