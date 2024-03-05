#pragma once

#include <memory>

#include "runtime/function/render/render_resource.h"
#include "runtime/function/render/render_pipeline.h"

namespace BJTUGE {

class RenderSystem {

public:
    void initialize();
    void tick(float delta_time);
    void clear();

private:
    std::shared_ptr<RenderResource> m_render_resource{nullptr};
    std::shared_ptr<RenderPipeline> m_render_pipeline{nullptr};
};

} // namespace BJTUGE
