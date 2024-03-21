#pragma once

#include "runtime/function/framework/component/component.h"

#include <glm/glm.hpp>
#include <memory>

namespace BJTUGE {

struct Rotation {
    // the axis of rotation
    glm::vec3 axis;
    // the angle of rotation (in degree)
    float angle;

    Rotation() : axis(0.0f, 0.0f, 0.0f), angle(0.0f) {}
    Rotation(float x, float y, float z, float angle_) : axis(x, y, z), angle(angle_) {}
};

class TransformComponent : public Component {

public:
    void initialize(std::weak_ptr<GObject> parent) override;

    void tick(float delta_time) override;

    glm::vec3 getPosition() const { return m_position; }
    Rotation  getRotation() const { return m_rotation; }
    glm::vec3 getScale() const { return m_scale; }

    void setPosition(glm::vec3 position);
    void setRotation(Rotation rotation);
    void setScale(glm::vec3 scale);

private:
    void updateModelMatrix();

private:
    // fundamental
    glm::vec3 m_position{0.0f};
    Rotation  m_rotation{};
    glm::vec3 m_scale{1.0f};
    // derived
    glm::mat4 m_transform_matrix{1.0f};
    glm::mat4 m_rotation_matrix{1.0f};
    glm::mat4 m_scale_matrix{1.0f};
    glm::mat4 m_model{1.0f};
};

} // namespace BJTUGE