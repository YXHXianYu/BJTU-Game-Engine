#pragma once

#include <vector>

#include "runtime/function/render/render_mesh.h"

namespace BJTUGE {

class RenderEntity {

public:
    // private:
    std::vector<RenderMesh> m_render_meshes;
};

} // namespace BJTUGE