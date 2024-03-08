#pragma once

#include "runtime/function/render/render_shader.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace BJTUGE {

class RenderResource;
class RenderCamera;

class RenderPipeline {

public:
    void initialize();

    void draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);
    void drawShadertoy();

private:
    std::unordered_map<std::string, std::shared_ptr<RenderShader>> m_render_shaders;
};

} // namespace BJTUGE
