#pragma once

#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_texture.h"

#include <vector>
#include <memory>

namespace BJTUGE {

class RenderEntity {

public:
    // private:
    std::vector<std::shared_ptr<RenderMesh>>    m_render_meshes;
    std::vector<std::shared_ptr<RenderTexture>> m_render_textures;
};

} // namespace BJTUGE