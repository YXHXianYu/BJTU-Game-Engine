#pragma once

#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_resource.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace BJTUGE {

class RenderPipeline {

public:
    void initialize();

    void draw(std::shared_ptr<RenderResource> render_resource);

private:
    std::unordered_map<std::string, std::shared_ptr<RenderShader>> m_render_shaders;
};

} // namespace BJTUGE
