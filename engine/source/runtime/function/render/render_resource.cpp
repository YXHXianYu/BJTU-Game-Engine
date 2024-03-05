#include "runtime/function/render/render_resource.h"

namespace BJTUGE {

void RenderResource::initialize() {

    std::vector<Vertex> vertices = {
        Vertex{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    };

    std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

    RenderEntity render_entity;
    render_entity.m_render_meshes.push_back(RenderMesh{vertices, indices});

    m_render_entities_map["1"] = render_entity;
}

} // namespace BJTUGE
