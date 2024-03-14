#include "runtime/function/framework/component/transform/transform_component.h"

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

void TransformComponent::updateModelMatrix() {
    m_model = m_transform_matrix * m_rotation_matrix * m_scale_matrix;
}

} // namespace BJTUGE