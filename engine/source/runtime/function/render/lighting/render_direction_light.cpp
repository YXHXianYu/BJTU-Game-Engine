#include "runtime/function/render/lighting/render_direction_light.h"

#include "runtime/function/render/mesh/render_mesh_base.h"

namespace BJTUGE {
void RenderDirectionLight::draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) {
    model = model * m_model;
    for (auto& [key, render_mesh] : m_render_meshes) {
        render_mesh->draw(shader, resource, model);
    }
}
} // namespace BJTUGE