#include "runtime/function/render/render_resource.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

    std::vector<Vertex> cube = {
        Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, Vertex{0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        Vertex{0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},   Vertex{0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},  Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},

        Vertex{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},  Vertex{0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},    Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},   Vertex{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},

        Vertex{-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},   Vertex{-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},  Vertex{-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},

        Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},    Vertex{0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},  Vertex{0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        Vertex{0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},   Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},

        Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, Vertex{0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},   Vertex{0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        Vertex{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},  Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},

        Vertex{-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},  Vertex{0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},    Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        Vertex{-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},   Vertex{-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0},
    };

    RenderEntity render_entity;
    render_entity.addRenderMesh(std::make_shared<RenderMesh>(vertices, indices));
    render_entity.addRenderMesh(std::make_shared<RenderMesh>(cube, std::vector<uint32_t>{}));
    render_entity.addRenderTexture(std::make_shared<RenderTexture>("./asset/textures/pixel-island.jpg"));
    render_entity.addRenderTexture(std::make_shared<RenderTexture>("./asset/textures/MinatoAqua4.png"));

    m_render_entities_map["basic"] = render_entity;

    m_render_entities_map["aris"] = load_entity_from_file("./asset/models/characters/aris/CH0200.fbx");
}

RenderEntity RenderResource::load_entity_from_file(const std::string& file_path) {
    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs);

    return RenderEntity{};
}

} // namespace BJTUGE
