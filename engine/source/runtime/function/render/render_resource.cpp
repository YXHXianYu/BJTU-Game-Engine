#include "runtime/function/render/render_resource.h"

#include "glm/ext/matrix_transform.hpp"
#include "runtime/function/render/render_mesh.h"
#include "runtime/function/render/render_entity.h"
#include "runtime/function/render/render_texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <functional>
#include <memory>
#include <string>

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

    auto render_mesh = std::make_shared<RenderMesh>(cube, std::vector<uint32_t>{}, std::vector<std::string>{});
    auto texture1    = std::make_shared<RenderTexture>("./asset/textures/pixel-island.jpg", "diffuse");
    auto texture2    = std::make_shared<RenderTexture>("./asset/textures/MinatoAqua4.png", "specular");
    render_mesh->addTexture("./asset/textures/pixel-island.jpg");
    render_mesh->addTexture("./asset/textures/MinatoAqua4.png");
    m_render_entities["cube"] = std::make_shared<RenderEntity>();
    m_render_entities["cube"]->addMesh("cube", render_mesh);

    auto cube_mesh = std::make_shared<RenderMesh>(cube, std::vector<uint32_t>{}, std::vector<std::string>{}) ;
    m_render_entities["light-cube"] = std::make_shared<RenderEntity>();
    m_render_entities["light-cube"]->addMesh("cube", cube_mesh);
    m_render_entities["light-cube"]->setModelMatrix(
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 1.0f)), glm::vec3(0.2f))
    );

    m_render_entities["aris"] = loadEntityFromFile("./asset/models/characters/aris/CH0200.fbx");
    m_render_entities["aris"]->setModelMatrix(
        glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.5f, 0.0f)), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    m_render_entities["aris"]->get("CH0200")->removeEntity("bone_root");
    m_render_entities["aris"]->get("CH0200")->removeEntity("Aris_Original_Weapon");
    m_render_entities["aris"]->get("CH0200")->removeEntity("CH0200_SkillProp_Outline");
    m_render_entities["aris"]->get("CH0200")->removeEntity("FX_Aris_Original_Weapon");
    m_render_entities["aris"]->get("CH0200")->removeEntity("HaloRoot");
    m_render_entities["aris"]->get("CH0200")->removeEntity("EX_Root");
    m_render_entities["aris"]->get("CH0200")->removeEntity("VoiceSourceObjectParent");
    m_render_entities["aris"]->get("CH0200")->removeEntity("AudioSourceObjectParent");

    m_render_entities["miyako"] = loadEntityFromFile("./asset/models/characters/miyako/CH0215.fbx");
    m_render_entities["miyako"]->setModelMatrix(
        glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    m_render_entities["miyako"]->get("CH0215")->removeEntity("bone_root");
    m_render_entities["miyako"]->get("CH0215")->removeEntity("CH0215_Prop_Outline");
    m_render_entities["miyako"]->get("CH0215")->removeEntity("Miyako_Original_Weapon");
    m_render_entities["miyako"]->get("CH0215")->removeEntity("HaloRoot");
    m_render_entities["miyako"]->get("CH0215")->removeEntity("EX_Root");

    m_render_entities["koharu"] = loadEntityFromFile("./asset/models/characters/koharu/Koharu_Original.fbx");
    m_render_entities["koharu"]->setModelMatrix(
        glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.5f, 0.0f)), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    m_render_entities["koharu"]->get("Koharu_Original")->removeEntity("bone_root");
    m_render_entities["koharu"]->get("Koharu_Original")->removeEntity("Koharu_Original_Book");
    m_render_entities["koharu"]->get("Koharu_Original")->removeEntity("Koharu_Original_Grenade");
    m_render_entities["koharu"]->get("Koharu_Original")->removeEntity("Koharu_Original_Weapon");
    m_render_entities["koharu"]->get("Koharu_Original")->removeEntity("Koharu_Weapon_Toggle");
    m_render_entities["koharu"]->get("Koharu_Original")->removeEntity("HaloRoot");
    m_render_entities["koharu"]->get("Koharu_Original")->removeEntity("Ex_Root");
}

void RenderResource::addTexture(const std::string& key, std::shared_ptr<RenderTexture> texture) {
    if (m_render_textures.find(key) != m_render_textures.end()) { return; }
    m_render_textures[key] = texture;
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
            addTexture(texture_path, texture);
            textures.push_back(texture_path);
        }
    };

    auto process_mesh = [&](aiMesh* mesh, const aiScene* scene) -> std::shared_ptr<RenderMesh> {
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

        return std::make_shared<RenderMesh>(vertices, indices, textures);
    };

    std::function<std::shared_ptr<RenderEntity>(aiNode*, const aiScene*, uint32_t)> process_node =
        [&](aiNode* node, const aiScene* scene, uint32_t level) -> std::shared_ptr<RenderEntity> {
        // if (level <= 2) {
        //     std::cout << level << " ";
        //     for (uint32_t i = 0; i <= level; i++) {
        //         std::cout << "-";
        //     }
        //     std::cout << " :" << node->mName.C_Str() << std::endl;
        // }

        auto entity = std::make_shared<RenderEntity>();
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            auto name = node->mName.C_Str();

            // if (level <= 4) {
            //     std::cout << level << " ";
            //     for (uint32_t i = 0; i <= level; i++) {
            //         std::cout << " ";
            //     }
            //     std::cout << "  => " << name << std::endl;
            // }

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

} // namespace BJTUGE
