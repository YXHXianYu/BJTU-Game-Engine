#include "spot_light.h"
#include "../render_mesh.h"

namespace BJTUGE {
    void SpotLight::draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) {
        model = model * m_model;
        for (auto& [key, render_mesh] : m_render_meshes) {
            render_mesh->draw(shader, resource, model);
        }
    }
}