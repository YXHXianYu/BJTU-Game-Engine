#include "runtime/function/render/render_resource.h"

#include <iostream>

namespace BJTUGE {

void RenderResource::initialize() {

    std::vector<Vertex> vertices = {
        Vertex{{0.9f, 0.9f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
        Vertex{{0.9f, -0.9f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        Vertex{{-0.9f, -0.9f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        Vertex{{-0.9f, 0.9f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    };
    std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

    RenderEntity render_entity;
    render_entity.m_render_meshes.push_back(std::make_shared<RenderMesh>(vertices, indices));
    // render_entity.m_render_textures.push_back(std::make_shared<RenderTexture>("./asset/textures/bug1.png", 0));
    render_entity.m_render_textures.push_back(std::make_shared<RenderTexture>("./asset/textures/pixel-island.jpg"));
    render_entity.m_render_textures.push_back(std::make_shared<RenderTexture>("./asset/textures/MinatoAqua4.png"));

    m_render_entities_map["basic"] = render_entity;
}

} // namespace BJTUGE
