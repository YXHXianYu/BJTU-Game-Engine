#include "runtime/function/render/render_entity.h"

#include "runtime/function/render/mesh/render_mesh_base.h"
#include "runtime/function/render/render_shader.h"

#include <iostream>

namespace BJTUGE {

void RenderEntity::draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) {
    model = model * m_model;
    for (auto& [key, render_mesh] : m_render_meshes) {
        render_mesh->draw(shader, resource, model);
    }
    for (auto& [key, render_entity] : m_render_entities) {
        render_entity->draw(shader, resource, model);
    }
}

void RenderEntity::output(uint32_t level) const {
    for (auto& [key, render_mesh] : m_render_meshes) {
        for (int j = 0; j < level; j++)
            std::cout << "  ";
        std::cout << "mesh: " << key << std::endl;
    }
    for (auto& [key, render_entity] : m_render_entities) {
        for (int j = 0; j < level; j++)
            std::cout << "  ";
        std::cout << "entity: " << key << std::endl;

        render_entity->output(level + 1);
    }
}

} // namespace BJTUGE