#include "runtime/function/framework/component/transform/transform_component.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace BJTUGE {

void TransformComponent::initialize(std::weak_ptr<GObject> parent_object) {
    m_parent_object = parent_object;
}

void TransformComponent::tick(float delta_time) {
    // TODO: get father and find corresponding components
    if (m_is_dirty) {
        // TODO: update rigidbody
        // TODO: update mesh
    }
}

void TransformComponent::setPosition(glm::vec3 position) {
    m_is_dirty         = true;
    m_position         = position;
    m_transform_matrix = glm::translate(glm::mat4(1.0f), m_position);
}
void TransformComponent::setRotation(Rotation rotation) {
    m_is_dirty        = true;
    m_rotation        = rotation;
    m_rotation_matrix = glm::toMat4(glm::quat(glm::radians(m_rotation.angle), m_rotation.axis));
}
void TransformComponent::setScale(glm::vec3 scale) {
    m_is_dirty     = true;
    m_scale        = scale;
    m_scale_matrix = glm::scale(glm::mat4(1.0f), m_scale);
}

void TransformComponent::updateModelMatrix() {
    m_model = m_transform_matrix * m_rotation_matrix * m_scale_matrix;
}

} // namespace BJTUGE