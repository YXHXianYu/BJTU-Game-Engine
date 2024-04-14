#pragma once

#include "runtime/function/input/input_system.h"
#include "runtime/function/render/render_shader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

class RenderResource;
class RenderCamera;

class RenderPipeline {

public:
    void initialize();

    void draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

    void tick(uint32_t GameCommand, std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

private:
    std::unordered_map<std::string, std::shared_ptr<RenderShader>> m_render_shaders;

    bool render_block{false};
    bool render_character{false};
    bool render_light{false};
    bool use_ortho{false};
    // drj
    bool render_square{false};
};

} // namespace BJTUGE
