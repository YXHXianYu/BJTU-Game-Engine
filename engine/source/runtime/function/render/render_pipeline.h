#pragma once

#include "runtime/function/input/input_system.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

class RenderResource;
class RenderCamera;
class RenderShader;
class RenderFramebuffer;

class RenderPipeline {

public:
    void initialize();

    void draw(std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

    void tick(uint32_t GameCommand, std::shared_ptr<RenderResource> resource, std::shared_ptr<RenderCamera> camera);

private:
    std::unordered_map<std::string, std::shared_ptr<RenderShader>> m_render_shaders;

    std::unordered_map<std::string, std::shared_ptr<RenderFramebuffer>> m_render_framebuffers;

    bool render_block{true};
    bool render_character{true};
    bool render_light{true};
    bool use_ortho{true};

    bool render_assignments{true};

    bool render_by_depth{false};
};

} // namespace BJTUGE
