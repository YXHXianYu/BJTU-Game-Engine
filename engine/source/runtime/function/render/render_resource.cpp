﻿#include "runtime/function/render/render_resource.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/render/lighting/render_direction_light.h"
#include "runtime/function/render/lighting/render_spot_light.h"
#include "runtime/function/render/mesh/face_info.h"
#include "runtime/function/render/mesh/render_mesh.h"
#include "runtime/function/render/mesh/render_mesh_blocks.h"
#include "runtime/function/render/minecraft_blocks/gmemory_buffer.h"
#include "runtime/function/render/minecraft_blocks/render_minecraft_blocks_manager.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/texture/render_texture.h"
#include "runtime/function/render/texture/render_texture_3d.h"
#include "runtime/function/render/texture/render_texture_base.h"
#include "runtime/function/window/window_system.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace BJTUGE {

void RenderResource::initialize() {

    m_render_entities["characters"] = loadCharacters();

    m_render_entities["assignments"] = loadAssignments();

    m_render_textures["minecraft_texture"]     = loadMinecraftTexture();
    m_render_textures["noise_texture"]         = loadNoiseTexture();
    m_render_textures["fractal_noise_texture"] = loadFractalNoiseTexture();

    m_render_minecraft_blocks_manager = std::make_shared<RenderMinecraftBlocksManager>();
    m_render_minecraft_blocks_manager->initialize();
    m_render_entities["minecraft_blocks"] = m_render_minecraft_blocks_manager->getEntity();

    m_render_entities["water"] = loadWater();

    m_render_entities["postprocess"] = loadPostprocessRectangle();

    loadLightingCubeToResource();
    updateLightSpaceMatrix();

    // === Key bind ===
    bindKeyboardEvent();
}

std::shared_ptr<RenderEntity> RenderResource::getEntity(const std::string& key) const {
    assert(hasEntity(key));
    return m_render_entities.at(key);
}

void RenderResource::registerTexture(const std::string& key, std::shared_ptr<RenderTextureBase> texture) {
    if (hasTexture(key)) { return; }
    m_render_textures[key] = texture;
}

std::shared_ptr<RenderTextureBase> RenderResource::getTexture(const std::string& key) const {
    assert(hasTexture(key));
    return m_render_textures.at(key);
}

std::shared_ptr<RenderEntity> RenderResource::loadEntityFromFile(const std::string& file_path) {
    const bool VERBOSE = false;
    std::cerr << "Begin to load model from file: " << file_path << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    Assimp::Importer importer;
    uint32_t         flags = aiProcess_Triangulate | aiProcess_GenNormals; // do not use `aiProcess_FlipUVs`, because we flip uv in stb_image

    const aiScene* scene = importer.ReadFile(file_path, flags);
    if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { scene = importer.ReadFile("./bin/" + file_path, flags); }
    if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        assert(false);
    }

    std::string file_directory = file_path.substr(0, file_path.find_last_of('/'));

    auto process_material = [&](aiMaterial* material, const aiScene* scene, aiTextureType type, std::string type_name,
                                std::vector<std::string>& textures) -> void {
        if (material->GetTextureCount(type) >= 2) {
            std::cerr << "Model have more than 1 " << type_name << " texture, only the first one will be loaded!" << std::endl;
        }
        for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
            aiString str;
            material->GetTexture(type, i, &str);

            auto                           tex = scene->GetEmbeddedTexture(str.C_Str());
            std::shared_ptr<RenderTexture> texture;
            if (tex) {
                texture = std::make_shared<RenderTexture>(tex, type_name);
            } else {
                texture = std::make_shared<RenderTexture>(file_directory + "/" + str.C_Str(), type_name);
            }
            std::string texture_path = file_directory + "/" + str.C_Str();
            registerTexture(texture_path, texture);
            textures.push_back(texture_path);
        }
    };

    auto process_mesh = [&](aiMesh* mesh, const aiScene* scene) -> std::shared_ptr<RenderMeshBase> {
        std::vector<Vertex>      vertices;
        std::vector<uint32_t>    indices;
        std::vector<std::string> textures;

        // vertex
        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            vertices.push_back({
                glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
                (mesh->mTextureCoords[0] ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} : glm::vec2{0.0f, 0.0f}),
            });
        }

        // face
        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // material
        auto material = scene->mMaterials[mesh->mMaterialIndex];

        process_material(material, scene, aiTextureType_DIFFUSE, "diffuse", textures);
        process_material(material, scene, aiTextureType_SPECULAR, "specular", textures);
        process_material(material, scene, aiTextureType_NORMALS, "normal", textures);
        process_material(material, scene, aiTextureType_HEIGHT, "height", textures);

        return std::shared_ptr<RenderMeshBase>(std::make_shared<RenderMesh>(vertices, indices, textures));
        // return std::static_pointer_cast<RenderMeshBase>(std::make_shared<RenderMesh>(vertices, indices, textures));
    };

    std::function<std::shared_ptr<RenderEntity>(aiNode*, const aiScene*, uint32_t)> process_node =
        [&](aiNode* node, const aiScene* scene, uint32_t level) -> std::shared_ptr<RenderEntity> {
        if (VERBOSE && level <= 2) {
            std::cout << level << " ";
            for (uint32_t i = 0; i <= level; i++) {
                std::cout << "-";
            }
            std::cout << " :" << node->mName.C_Str() << std::endl;
        }

        auto entity = std::make_shared<RenderEntity>();
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            auto name = node->mName.C_Str();

            if (VERBOSE && level <= 4) {
                std::cout << level << " ";
                for (uint32_t i = 0; i <= level; i++) {
                    std::cout << " ";
                }
                std::cout << "  Mesh => " << name << std::endl;
            }

            if (entity->hasMesh(name)) {
                uint32_t index = 0;
                while (entity->hasMesh(name + std::to_string(index))) {
                    index++;
                }
                entity->addMesh(name + std::to_string(index), process_mesh(scene->mMeshes[node->mMeshes[i]], scene));
            } else {
                entity->addMesh(name, process_mesh(scene->mMeshes[node->mMeshes[i]], scene));
            }
        }
        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            auto name = node->mChildren[i]->mName.C_Str();
            if (entity->hasEntity(name)) {
                uint32_t index = 0;
                while (entity->hasEntity(name + std::to_string(index))) {
                    index++;
                }
                entity->setEntity(name + std::to_string(index), process_node(node->mChildren[i], scene, level + 1));
            } else {
                entity->setEntity(name, process_node(node->mChildren[i], scene, level + 1));
            }
        }
        return entity;
    };

    auto result   = process_node(scene->mRootNode, scene, 0);
    auto stop     = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cerr << "Finished in " << duration.count() << " ms" << std::endl;

    return result;
}

/**
 * If you want to add a minecraft texture, please modify the following code:
 * 1. RenderResource::loadMinecraftTexture()
 * 2. BlockId, add a new block id before "water" field (in function/framework/component/block_manager/block_manager_component.h)
 * 3. BlockManagerComponent.m_block_properties (in the same file)
 * 4. GLSL Definition: MATERIAL_ID_SUM (in shader/include/config.glsl)
 */
std::shared_ptr<RenderTextureBase> RenderResource::loadMinecraftTexture() {
    auto textures_path = std::vector<std::string>{
        "",
        "./asset/textures/blocks/stone.png",
        "./asset/textures/blocks/grass.png",
        "./asset/textures/blocks/dirt.png",
        "./asset/textures/blocks/cobblestone.png",
        "./asset/textures/blocks/oak_planks.png",
        "./asset/textures/blocks/oak_log.png",
        "./asset/textures/blocks/oak_leaves.png",
        "./asset/textures/blocks/red_wool.png",
        "./asset/textures/blocks/red_wool_brighter.png",
        "",
    };
    return std::shared_ptr<RenderTextureBase>(std::make_shared<RenderTexture3D>(textures_path));
}

std::shared_ptr<RenderTextureBase> RenderResource::loadNoiseTexture() {
    auto texture_path = "./asset/textures/noise.png";
    return std::shared_ptr<RenderTextureBase>(std::make_shared<RenderTexture>(texture_path, "default", "linear"));
}

std::shared_ptr<RenderTextureBase> RenderResource::loadFractalNoiseTexture() {
    auto texture_path = "./asset/textures/fractal_noise.png";
    return std::shared_ptr<RenderTextureBase>(std::make_shared<RenderTexture>(texture_path, "default", "linear"));
}

/* ===== For Test ===== */

std::shared_ptr<RenderEntity> RenderResource::loadMinecraftBlocks() {
    std::vector<FaceInfo> blocks(100);
    for (int i = 0; i < 100; i++) {
        blocks[i] = FaceInfo{2.0f, 2.0f, (i + 2.0f) * 1.0f, (float)(i % 6), (float)(i % 4)};
    }
    auto mesh_blocks = std::make_shared<RenderMeshBlocks>(blocks);
    auto entity      = std::make_shared<RenderEntity>();
    entity->addMesh("", mesh_blocks);
    return entity;
}

void RenderResource::loadLightingCubeToResource() {
    auto create_light_cube = [&](glm::vec3 position, glm::vec3 color) -> std::shared_ptr<RenderSpotLight> {
        auto light_cube = std::make_shared<RenderSpotLight>();
        light_cube->addMesh("cube", loadCubeMesh());
        std::dynamic_pointer_cast<RenderMesh>(light_cube->getMesh("cube"))->setModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
        std::dynamic_pointer_cast<RenderMesh>(light_cube->getMesh("cube"))->setDiffuseColor(color);
        light_cube->setPosition(position);
        light_cube->setColor(color);
        return light_cube;
    };

    auto create_direction_light_cube = [&](glm::vec3 direction, glm::vec3 color) -> std::shared_ptr<RenderDirectionLight> {
        auto direction_light_cube = std::make_shared<RenderDirectionLight>();
        direction_light_cube->addMesh("cube", loadCubeMesh());
        std::dynamic_pointer_cast<RenderMesh>(direction_light_cube->getMesh("cube"))
            ->setModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
        std::dynamic_pointer_cast<RenderMesh>(direction_light_cube->getMesh("cube"))->setDiffuseColor(color);
        direction_light_cube->setDirection(direction);
        direction_light_cube->setColor(color);
        return direction_light_cube;
    };

    m_spot_lights["light_cube_1"] = create_light_cube(glm::vec3(-0.5f, -0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m_spot_lights["light_cube_2"] = create_light_cube(glm::vec3(0.0f, 0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_spot_lights["light_cube_3"] = create_light_cube(glm::vec3(0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    m_direction_lights["direction_light_cube_1"] = create_direction_light_cube(m_sun_light_direction, glm::vec3(m_sun_light_intensity));
}

std::shared_ptr<RenderMeshBase> RenderResource::loadCubeMesh() {
    std::vector<Vertex> cube = {
        Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f}, Vertex{0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},
        Vertex{0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},   Vertex{0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},
        Vertex{-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},  Vertex{-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},

        Vertex{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},   Vertex{0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
        Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},     Vertex{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
        Vertex{-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},    Vertex{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},

        Vertex{-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},   Vertex{-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f}, Vertex{-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f},  Vertex{-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},

        Vertex{0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},     Vertex{0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},   Vertex{0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        Vertex{0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},    Vertex{0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},

        Vertex{-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f}, Vertex{0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},   Vertex{0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        Vertex{-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f},  Vertex{-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f},

        Vertex{-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},   Vertex{0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},     Vertex{0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
        Vertex{-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},    Vertex{-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0},
    };

    return std::shared_ptr<RenderMeshBase>(std::make_shared<RenderMesh>(cube, std::vector<uint32_t>{}, std::vector<std::string>{}));
}

std::shared_ptr<RenderEntity> RenderResource::loadCharacters() {
    auto entity       = std::make_shared<RenderEntity>();
    auto delta_height = -0.25f;

    entity->addEntity("aris", RenderResource::loadEntityFromFile("./asset/models/characters/aris/CH0200.fbx"));
    entity->get("aris")->setModelMatrix(glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
                                                       glm::vec3(-1.0f, 0.0f, delta_height)));
    entity->get("aris")->get("CH0200")->removeEntity("bone_root");
    entity->get("aris")->get("CH0200")->removeEntity("Aris_Original_Weapon");
    entity->get("aris")->get("CH0200")->removeEntity("CH0200_SkillProp_Outline");
    entity->get("aris")->get("CH0200")->removeEntity("FX_Aris_Original_Weapon");
    entity->get("aris")->get("CH0200")->removeEntity("HaloRoot");
    entity->get("aris")->get("CH0200")->removeEntity("EX_Root");
    entity->get("aris")->get("CH0200")->removeEntity("VoiceSourceObjectParent");
    entity->get("aris")->get("CH0200")->removeEntity("AudioSourceObjectParent");

    entity->addEntity("miyako", loadEntityFromFile("./asset/models/characters/miyako/CH0215.fbx"));
    entity->get("miyako")->setModelMatrix(
        glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 0.0f, delta_height)));
    entity->get("miyako")->get("CH0215")->removeEntity("bone_root");
    entity->get("miyako")->get("CH0215")->removeEntity("CH0215_Prop_Outline");
    entity->get("miyako")->get("CH0215")->removeEntity("Miyako_Original_Weapon");
    entity->get("miyako")->get("CH0215")->removeEntity("HaloRoot");
    entity->get("miyako")->get("CH0215")->removeEntity("EX_Root");

    entity->addEntity("koharu", loadEntityFromFile("./asset/models/characters/koharu/Koharu_Original.fbx"));
    entity->get("koharu")->setModelMatrix(
        glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 0.0f, delta_height)));
    entity->get("koharu")->get("Koharu_Original")->removeEntity("bone_root");
    entity->get("koharu")->get("Koharu_Original")->removeEntity("Koharu_Original_Book");
    entity->get("koharu")->get("Koharu_Original")->removeEntity("Koharu_Original_Grenade");
    entity->get("koharu")->get("Koharu_Original")->removeEntity("Koharu_Original_Weapon");
    entity->get("koharu")->get("Koharu_Original")->removeEntity("Koharu_Weapon_Toggle");
    entity->get("koharu")->get("Koharu_Original")->removeEntity("HaloRoot");
    entity->get("koharu")->get("Koharu_Original")->removeEntity("Ex_Root");

    return entity;
}

std::shared_ptr<RenderEntity> RenderResource::loadPlainBlocks() {
    auto texture = std::make_shared<RenderTexture>("./asset/textures/blocks/grass.png", "diffuse");

    auto model = loadEntityFromFile("./asset/models/block/block.obj");
    auto mesh  = model->get("Cube")->getMesh("Cube");
    std::dynamic_pointer_cast<RenderMesh>(mesh)->addTexture("./asset/textures/blocks/grass.png", texture,
                                                            std::shared_ptr<RenderResource>(this));
    assert(mesh);

    auto f_entity = std::make_shared<RenderEntity>();

    for (int i = -25; i < 25; i++) {
        for (int j = -25; j < 25; j++) {
            std::shared_ptr<RenderEntity> entity = std::make_shared<RenderEntity>();
            entity->addMesh("Cube", mesh);
            auto str = "block" + std::to_string(i) + "_" + std::to_string(j);
            f_entity->addEntity(str, entity);
            entity->setModelMatrix(
                glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f)), glm::vec3(i * (2.0f), -3.5f, j * (2.0f))));
        }
    }

    return f_entity;
}

std::shared_ptr<RenderEntity> RenderResource::loadWater() {
    // mesh
    const std::vector<Vertex> face_vertex = {
        Vertex{0.5, 0.5, -0.5, 0, 1, 0, 0.625, 0.5},  Vertex{-0.5, 0.5, -0.5, 0, 1, 0, 0.875, 0.5},
        Vertex{-0.5, 0.5, 0.5, 0, 1, 0, 0.875, 0.75}, Vertex{0.5, 0.5, 0.5, 0, 1, 0, 0.625, 0.75},
        Vertex{0.5, 0.5, -0.5, 0, 1, 0, 0.625, 0.5},  Vertex{-0.5, 0.5, 0.5, 0, 1, 0, 0.875, 0.5},
    };
    auto face_mesh =
        std::shared_ptr<RenderMeshBase>(std::make_shared<RenderMesh>(face_vertex, std::vector<uint32_t>{}, std::vector<std::string>{}));

    // entity
    auto entity = std::make_shared<RenderEntity>();

    for (int i = -15; i <= 15; i++) {
        for (int j = 2; j <= 15; j++) {
            auto face = std::make_shared<RenderEntity>();
            face->addMesh("face", face_mesh);
            face->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, j)));
            entity->addEntity("face" + std::to_string(i) + ";" + std::to_string(j), face);
        }
    }

    entity->setModelMatrix(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)), glm::vec3(0.0f, -1.0f, 0.0f)));

    return entity;
}

std::shared_ptr<RenderEntity> RenderResource::loadPostprocessRectangle() {
    // mesh
    std::vector<Vertex> rect = {
        Vertex{-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, Vertex{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        Vertex{1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},

        Vertex{-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, Vertex{1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        Vertex{1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
    };

    auto mesh = std::shared_ptr<RenderMeshBase>(std::make_shared<RenderMesh>(rect, std::vector<uint32_t>{}, std::vector<std::string>{}));

    // entity
    auto entity = std::make_shared<RenderEntity>();
    entity->addMesh("rect", mesh);
    return entity;
}

/* ===== Assignments ===== */

std::shared_ptr<RenderEntity> RenderResource::loadAssignments() {
    auto entity = std::make_shared<RenderEntity>();
    entity->addEntity("drj", loadSquareLovekdl());
    entity->addEntity("fjq", loadCubesFJQ());
    // entity->addEntity("cjx", loadCatsCJX());
    entity->addEntity("creeper", loadCreeper());
    // entity->addEntity("house", loadHouse());
    entity->addEntity("tree", loadTree());
    entity->addEntity("sheep", loadSheep());
    entity->addEntity("sword", loadSword());

    return entity;
}

// drj
std::shared_ptr<RenderEntity> RenderResource::load2DShape() {
    auto entity = std::make_shared<RenderEntity>();

    std::vector<Vertex> square = {
        Vertex{-1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, Vertex{0.0f, 1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        Vertex{1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // 第一个三角形
        Vertex{1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},  Vertex{0.0f, -1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        Vertex{-1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // 第二个三角形
        Vertex{1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},  Vertex{1.5f, -1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        Vertex{2.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // 第三个三角形，稍微向外凸起
        Vertex{-1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, Vertex{-1.5f, 1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        Vertex{-2.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // 第四个三角形，稍微向外凹陷
        Vertex{-1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, Vertex{-2.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        Vertex{0.0f, -2.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f}, // 第五个三角形，形成一个尖角
        Vertex{1.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},  Vertex{2.0f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        Vertex{0.0f, 2.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f} // 第六个三角形，形成一个尖角
    };

    const float coef = 0.5f;
    for (auto& vertex : square) {
        vertex.position *= coef;
    }

    entity->addMesh(
        "square", std::shared_ptr<RenderMeshBase>(std::make_shared<RenderMesh>(square, std::vector<uint32_t>{}, std::vector<std::string>{})));
    return entity;
}

// drj
std::shared_ptr<RenderEntity> RenderResource::loadSquareLovekdl() {
    auto entity = std::make_shared<RenderEntity>();
    entity->addEntity("origin", load2DShape());
    entity->addEntity("scale", load2DShape());
    entity->addEntity("reflect", load2DShape());
    entity->addEntity("shear", load2DShape());
    entity->addEntity("rotate", load2DShape());
    entity->addEntity("translate", load2DShape());
    entity->addEntity("combine", load2DShape());
    entity->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 8.0f, -15.0f)));

    entity->get("translate")
        ->setModelMatrix(glm::translate(glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 3.0f, 0.0f)));
    entity->get("rotate")->setModelMatrix(
        glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 0.0f)), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    entity->get("scale")->setModelMatrix(
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, 0.0f, 0.0f)), glm::vec3(0.5f, 0.5f, 0.5f)));
    entity->get("reflect")->setModelMatrix(
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, 0.0f)), glm::vec3(-1.0f, 1.0f, 1.0f)));

    float     shearX      = 1.0f; // X轴切变量
    float     shearY      = 0.5f; // Y轴切变量
    glm::mat4 shearMatrix = glm::mat4(1.0f);
    shearMatrix[1][0]     = shearX;
    shearMatrix[0][1]     = shearY;
    entity->get("shear")->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f)) * shearMatrix);

    glm::mat4 rotateMat    = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 scaleMat     = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    entity->get("combine")->setModelMatrix(translateMat * glm::translate(rotateMat, glm::vec3(15.6f, -9.0f, 0.0f)) * scaleMat);

    return entity;
}

std::shared_ptr<RenderEntity> RenderResource::loadCubesFJQ() {
    auto entity = std::make_shared<RenderEntity>();

    float initialX = -15.0f;
    float initialY = 5.0f;
    float initialZ = -15.0f;
    float deltaX   = 3.0f;

    auto path = "./asset/models/cube/cube.obj";

    auto model         = RenderResource::loadEntityFromFile(path);
    auto model_wrapper = [&]() {
        auto model_entity = std::make_shared<RenderEntity>();
        model_entity->addEntity("cube", model);
        return model_entity;
    };

    entity->addEntity("OriginalCube", model_wrapper());
    entity->getEntity("OriginalCube")->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(initialX, initialY, initialZ)));

    entity->addEntity("ScaledCube", model_wrapper());
    glm::mat4 scaledMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initialX + deltaX, initialY, initialZ));
    scaledMatrix           = glm::scale(scaledMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    entity->getEntity("ScaledCube")->setModelMatrix(scaledMatrix);

    entity->addEntity("TranslatedCube", model_wrapper());
    glm::mat4 translatedMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initialX + 2 * deltaX, initialY, initialZ));
    translatedMatrix           = glm::translate(translatedMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    entity->getEntity("TranslatedCube")->setModelMatrix(translatedMatrix);

    entity->addEntity("RotatedXCube", model_wrapper());
    glm::mat4 rotatedXMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initialX + 3 * deltaX, initialY, initialZ));
    rotatedXMatrix           = glm::rotate(rotatedXMatrix, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    entity->getEntity("RotatedXCube")->setModelMatrix(rotatedXMatrix);

    entity->addEntity("RotatedYCube", model_wrapper());
    glm::mat4 rotatedYMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initialX + 4 * deltaX, initialY, initialZ));
    rotatedYMatrix           = glm::rotate(rotatedYMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    entity->getEntity("RotatedYCube")->setModelMatrix(rotatedYMatrix);

    entity->addEntity("RotatedZCube", model_wrapper());
    glm::mat4 rotatedZMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initialX + 5 * deltaX, initialY, initialZ));
    rotatedZMatrix           = glm::rotate(rotatedZMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    entity->getEntity("RotatedZCube")->setModelMatrix(rotatedZMatrix);

    entity->addEntity("CompositeTransformCube", model_wrapper());
    glm::mat4 compositeMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(initialX + 6 * deltaX, initialY, initialZ));
    compositeMatrix           = glm::scale(compositeMatrix, glm::vec3(0.75f, 0.75f, 0.75f));
    compositeMatrix           = glm::rotate(compositeMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    entity->getEntity("CompositeTransformCube")->setModelMatrix(compositeMatrix);

    return entity;
}

std::shared_ptr<RenderEntity> RenderResource::loadCatsCJX() {
    auto entity = std::make_shared<RenderEntity>();

    float initialX = -15.0f;
    float initialY = 5.0f;
    float initialZ = -15.0f;
    float deltaX   = 3.0f;

    auto path = "./asset/models/cat/cat.obj";

    auto model         = RenderResource::loadEntityFromFile(path);
    auto model_wrapper = [&]() {
        auto model_entity = std::make_shared<RenderEntity>();
        model_entity->addEntity("cube", model);
        return model_entity;
    };

    const auto scale_coef      = 0.05f;
    const auto normalize_scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_coef));

    auto wrapper = [&](glm::mat4 transform, float delta) {
        return glm::translate(glm::mat4(1.0f), glm::vec3(initialX, initialY, initialZ + delta * deltaX)) * transform * normalize_scale;
    };

    entity->addEntity("OriginalCat", model_wrapper());
    entity->addEntity("ScaledCat", model_wrapper());
    entity->addEntity("TranslatedCat", model_wrapper());
    entity->addEntity("RotatedXCat", model_wrapper());
    entity->addEntity("RotatedYCat", model_wrapper());
    entity->addEntity("RotatedZCat", model_wrapper());
    entity->addEntity("CompositeTransformCat", model_wrapper());

    // 平移移到初始位置，并旋转至猫猫站立
    glm::mat4 toOriginalMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    entity->getEntity("OriginalCat")->setModelMatrix(wrapper(toOriginalMatrix, 0));

    glm::mat4 scaledMatrix = glm::scale(toOriginalMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    entity->getEntity("ScaledCat")->setModelMatrix(wrapper(scaledMatrix, 1));

    glm::mat4 translatedMatrix = glm::translate(toOriginalMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
    entity->getEntity("TranslatedCat")->setModelMatrix(wrapper(translatedMatrix, 2));

    // 分别围绕 X、Y、Z 轴旋转了45度
    glm::mat4 rotatedXMatrix = glm::translate(toOriginalMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
    rotatedXMatrix           = glm::rotate(rotatedXMatrix, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    entity->getEntity("RotatedXCat")->setModelMatrix(wrapper(rotatedXMatrix, 3));

    glm::mat4 rotatedYMatrix = glm::translate(toOriginalMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
    rotatedYMatrix           = glm::rotate(rotatedYMatrix, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    entity->getEntity("RotatedYCat")->setModelMatrix(wrapper(rotatedYMatrix, 4));

    glm::mat4 rotatedZMatrix = glm::translate(toOriginalMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
    rotatedZMatrix           = glm::rotate(rotatedZMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    entity->getEntity("RotatedZCat")->setModelMatrix(wrapper(rotatedZMatrix, 5));

    // 先向Z轴平移1，再缩放了0.5倍然后绕Z轴旋转了30度
    glm::mat4 compositeMatrix = glm::translate(toOriginalMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
    compositeMatrix           = glm::scale(compositeMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
    compositeMatrix           = glm::rotate(compositeMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    entity->getEntity("CompositeTransformCat")->setModelMatrix(wrapper(compositeMatrix, 6));

    return entity;
}
std::shared_ptr<RenderEntity> RenderResource::loadSword() {
    auto entity = std::make_shared<RenderEntity>();

    float initialX = 6.5f;
    float initialY = -0.35f;
    float initialZ = 3.0f;
    float deltaX   = 3.0f;

    auto path = "./asset/models/sword/DiamondSword.obj";

    auto model         = RenderResource::loadEntityFromFile(path);
    auto model_wrapper = [&]() {
        auto model_entity = std::make_shared<RenderEntity>();
        model_entity->addEntity("cube", model);
        return model_entity;
    };

    const auto scale_coef      = 0.075f;
    const auto normalize_scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_coef));

    auto wrapper = [&](glm::mat4 transform, float delta) {
        return glm::translate(glm::mat4(1.0f), glm::vec3(initialX, initialY, initialZ + delta * deltaX)) * transform * normalize_scale;
    };

    entity->addEntity("RotatedSword", model_wrapper());

    glm::mat4 rotatedXMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.5f, 0.8f));
    entity->getEntity("RotatedSword")->setModelMatrix(wrapper(rotatedXMatrix, 1));
    return entity;
}
std::shared_ptr<RenderEntity> RenderResource::loadSheep() {
    auto entity = std::make_shared<RenderEntity>();

    float initialX = 5.0f;
    float initialY = 0.25f;
    float initialZ = 1.5f;
    float deltaX   = 3.0f;

    auto path = "./asset/models/sheep/sheep.obj";

    auto model         = RenderResource::loadEntityFromFile(path);
    auto model_wrapper = [&]() {
        auto model_entity = std::make_shared<RenderEntity>();
        model_entity->addEntity("cube", model);
        return model_entity;
    };

    const auto scale_coef      = 0.5f;
    const auto normalize_scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_coef));

    auto wrapper = [&](glm::mat4 transform, float delta) {
        return glm::translate(glm::mat4(1.0f), glm::vec3(initialX, initialY, initialZ + delta * deltaX)) * transform * normalize_scale;
    };

    entity->addEntity("ScaledSheep", model_wrapper());

    entity->getEntity("ScaledSheep")->setModelMatrix(wrapper(glm::mat4(1.0f), 1));
    return entity;
}
std::shared_ptr<RenderEntity> RenderResource::loadTree() {
    auto entity = std::make_shared<RenderEntity>();

    float initialX = 10.0f;
    float initialY = -0.25f;
    float initialZ = -5.0f;
    float deltaX   = 3.0f;

    auto path = "./asset/models/tree/tree.obj";

    auto model         = RenderResource::loadEntityFromFile(path);
    auto model_wrapper = [&]() {
        auto model_entity = std::make_shared<RenderEntity>();
        model_entity->addEntity("cube", model);
        return model_entity;
    };

    const auto scale_coef      = 0.5f;
    const auto normalize_scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_coef));

    auto wrapper = [&](glm::mat4 transform, float delta) {
        return glm::translate(glm::mat4(1.0f), glm::vec3(initialX, initialY, initialZ + delta * deltaX)) * transform * normalize_scale;
    };

    entity->addEntity("ScaledHouse", model_wrapper());

    entity->getEntity("ScaledHouse")->setModelMatrix(wrapper(glm::mat4(1.0f), 1));
    return entity;
}
std::shared_ptr<RenderEntity> RenderResource::loadHouse() {
    auto entity = std::make_shared<RenderEntity>();

    float initialX = 5.0f;
    float initialY = -0.25f;
    float initialZ = -10.0f;
    float deltaX   = 3.0f;

    auto path = "./asset/models/house/house.obj";

    auto model         = RenderResource::loadEntityFromFile(path);
    auto model_wrapper = [&]() {
        auto model_entity = std::make_shared<RenderEntity>();
        model_entity->addEntity("cube", model);
        return model_entity;
    };

    const auto scale_coef      = 0.5f;
    const auto normalize_scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_coef));

    auto wrapper = [&](glm::mat4 transform, float delta) {
        return glm::translate(glm::mat4(1.0f), glm::vec3(initialX, initialY, initialZ + delta * deltaX)) * transform * normalize_scale;
    };

    entity->addEntity("RotatedHouse", model_wrapper());

    glm::mat4 rotatedXMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    entity->getEntity("RotatedHouse")->setModelMatrix(wrapper(rotatedXMatrix, 1));

    return entity;
}
std::shared_ptr<RenderEntity> RenderResource::loadCreeper() {
    auto entity = std::make_shared<RenderEntity>();

    float initialX = 7.0f;
    float initialY = 0.3f;
    float initialZ = -3.0f;
    float deltaX   = 3.0f;

    auto path = "./asset/models/creeper/creeper.obj";

    auto model         = RenderResource::loadEntityFromFile(path);
    auto model_wrapper = [&]() {
        auto model_entity = std::make_shared<RenderEntity>();
        model_entity->addEntity("cube", model);
        return model_entity;
    };

    const auto scale_coef      = 0.6f;
    const auto normalize_scale = glm::scale(glm::mat4(1.0f), glm::vec3(scale_coef));

    auto wrapper = [&](glm::mat4 transform, float delta) {
        return glm::translate(glm::mat4(1.0f), glm::vec3(initialX, initialY, initialZ + delta * deltaX)) * transform * normalize_scale;
    };

    entity->addEntity("RotatedCreeper", model_wrapper());

    glm::mat4 rotatedXMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(320.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    entity->getEntity("RotatedCreeper")->setModelMatrix(wrapper(rotatedXMatrix, 1));
    return entity;
}
void RenderResource::bindKeyboardEvent() {
    g_runtime_global_context.m_window_system->registerOnKeyFunc(
        std::bind(&RenderResource::onKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void RenderResource::updateLightSpaceMatrix() { // Change shadow map matrix in this place
    glm::mat4 light_projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, m_light_space_near, m_light_space_far);
    glm::mat4 light_view       = glm::lookAt(glm::vec3((-m_sun_light_direction) * 10.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_light_space_matrix       = light_projection * light_view;
}

void RenderResource::onKey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            default: {
                break;
            }
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_Z: {
                if (m_sun_light_intensity < 1.0) m_sun_light_intensity += 0.05;
                m_direction_lights["direction_light_cube_1"]->setColor(glm::vec3(m_sun_light_intensity));
                break;
            }
            case GLFW_KEY_X: {
                if (m_sun_light_intensity > 0.4) m_sun_light_intensity -= 0.05;
                m_direction_lights["direction_light_cube_1"]->setColor(glm::vec3(m_sun_light_intensity));
                break;
            }
            case GLFW_KEY_R: {
                m_sun_light_direction.x += 0.1;
                m_sun_light_direction = glm::normalize(m_sun_light_direction);
                updateLightSpaceMatrix();
                m_direction_lights["direction_light_cube_1"]->setDirection(m_sun_light_direction);
                break;
            }
            case GLFW_KEY_T: {
                m_sun_light_direction.x -= 0.1;
                m_sun_light_direction = glm::normalize(m_sun_light_direction);
                updateLightSpaceMatrix();
                m_direction_lights["direction_light_cube_1"]->setDirection(m_sun_light_direction);
                break;
            }
            default: {
                break;
            }
        }
    }
}

} // namespace BJTUGE
