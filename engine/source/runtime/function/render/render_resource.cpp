#include "runtime/function/render/render_resource.h"

#include "runtime/function/render/lighting/render_direction_light.h"
#include "runtime/function/render/lighting/render_spot_light.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_mesh_blocks.h"
#include "runtime/function/render/render_texture.h"
#include "runtime/function/render/render_texture_3d.h"
#include "runtime/function/render/render_texture_base.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace BJTUGE {

void RenderResource::initialize() {

    m_render_entities["model"] = std::make_shared<RenderEntity>();
    m_render_entities["model"]->addEntity("characters", loadCharacters());

    m_render_entities["minecraft_blocks"]  = loadMinecraftBlocks();
    m_render_textures["minecraft_texture"] = loadMinecraftTexture();

    loadLightingCubeToResource();
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

    const bool VERBOSE = false;

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
                std::cout << "  => " << name << std::endl;
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

    return process_node(scene->mRootNode, scene, 0);
}

std::shared_ptr<RenderTextureBase> RenderResource::loadMinecraftTexture() {
    auto textures_path = std::vector<std::string>{
        "./asset/textures/blocks/grass.png",       "./asset/textures/blocks/stone.png",      "./asset/textures/blocks/dirt.png",
        "./asset/textures/blocks/cobblestone.png", "./asset/textures/blocks/oak_planks.png", "./asset/textures/blocks/oak_log.png",
        "./asset/textures/blocks/oak_leaves.png",
    };
    return std::shared_ptr<RenderTextureBase>(std::make_shared<RenderTexture3D>(textures_path));
}

/* ===== For Test ===== */

std::shared_ptr<RenderEntity> RenderResource::loadMinecraftBlocks() {
    std::vector<BlockInfo> blocks(100);
    for (int i = 0; i < 100; i++) {
        if (i <= 2)
            blocks[i] = BlockInfo{0.0f, 0.0f, (i + 1.0f) * 1.0f, (float)(i % 6), -1.0f};
        else
            blocks[i] = BlockInfo{0.0f, 0.0f, (i + 1.0f) * 1.0f, (float)(i % 6), (float)(i % 4)};
    }
    auto mesh_blocks = std::make_shared<RenderMeshBlocks>(blocks);
    auto entity      = std::make_shared<RenderEntity>();
    entity->addMesh("", mesh_blocks);
    return entity;
}

void RenderResource::loadLightingCubeToResource() {
    auto mesh = loadCubeMesh();

    auto create_light_cube = [&](glm::vec3 position, glm::vec3 color) -> std::shared_ptr<RenderSpotLight> {
        auto light_cube = std::make_shared<RenderSpotLight>();
        light_cube->addMesh("cube", mesh);
        std::dynamic_pointer_cast<RenderMesh>(light_cube->getMesh("cube"))->setModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
        light_cube->setPosition(position);
        light_cube->setColor(color);
        return light_cube;
    };

    auto create_direction_light_cube = [&](glm::vec3 direction, glm::vec3 color) -> std::shared_ptr<RenderDirectionLight> {
        auto direction_light_cube = std::make_shared<RenderDirectionLight>();
        direction_light_cube->addMesh("cube", mesh);
        std::dynamic_pointer_cast<RenderMesh>(direction_light_cube->getMesh("cube"))->setModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
        direction_light_cube->setDirection(direction);
        direction_light_cube->setColor(color);
        return direction_light_cube;
    };

    m_spot_lights["light_cube_1"] = create_light_cube(glm::vec3(-0.5f, -0.5f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m_spot_lights["light_cube_2"] = create_light_cube(glm::vec3(0.0f, 0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_spot_lights["light_cube_3"] = create_light_cube(glm::vec3(0.5f, -0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    m_direction_lights["direction_light_cube_1"] = create_direction_light_cube(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(1.0f, 0.8f, 0.6f));
}

std::shared_ptr<RenderMeshBase> RenderResource::loadCubeMesh() {
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

    return std::shared_ptr<RenderMeshBase>(std::make_shared<RenderMesh>(cube, std::vector<uint32_t>{}, std::vector<std::string>{}));
}

std::shared_ptr<RenderEntity> RenderResource::loadCharacters() {
    auto entity = std::make_shared<RenderEntity>();

    entity->addEntity("aris", RenderResource::loadEntityFromFile("./asset/models/characters/aris/CH0200.fbx"));
    entity->get("aris")->setModelMatrix(
        glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(-1.0f, 0.0f, 0.0f)));
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
        glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, 0.0f, 0.0f)));
    entity->get("miyako")->get("CH0215")->removeEntity("bone_root");
    entity->get("miyako")->get("CH0215")->removeEntity("CH0215_Prop_Outline");
    entity->get("miyako")->get("CH0215")->removeEntity("Miyako_Original_Weapon");
    entity->get("miyako")->get("CH0215")->removeEntity("HaloRoot");
    entity->get("miyako")->get("CH0215")->removeEntity("EX_Root");

    entity->addEntity("koharu", loadEntityFromFile("./asset/models/characters/koharu/Koharu_Original.fbx"));
    entity->get("koharu")->setModelMatrix(
        glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f)));
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

} // namespace BJTUGE
