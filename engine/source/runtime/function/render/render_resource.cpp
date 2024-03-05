#include "runtime/function/render/render_resource.h"

namespace BJTUGE {

void RenderResource::initialize() {

    RenderEntity render_entity;

    render_entity.m_render_meshes.push_back(RenderMesh{{
                                                           Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                                           Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                                           Vertex{{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                                       },
                                                       {0, 1, 2}});

    m_render_entities_map["1"] = render_entity;
}

} // namespace BJTUGE
