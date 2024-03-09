#include "runtime/function/render/render_entity.h"

#include "runtime/function/render/render_shader.h"
#include "runtime/function/render/render_mesh.h"

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

} // namespace BJTUGE