#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

namespace BJTUGE {

struct CameraCreateInfo {
    glm::vec3 position{-2.0f, 2.0f, 1.5f};
    float     yaw{0.0f};
    float     pitch{-30.0f};

    float fovy{90.0f};
    float aspect{16.0f / 9.0f};
    float near{0.1f};
    float far{100.0f}; // too large far may cause precision problem

    float move_speed{0.001f};
    float mouse_sensitivity{0.3f};
};

class RenderCamera {

public:
    RenderCamera(CameraCreateInfo camera_info);

    // getter
    glm::mat4 getViewMatrix() {
        updateViewMatrix();
        return m_view_matrix;
    }
    glm::mat4 getProjectionMatrix() {
        updateProjectionMatrix();
        return m_projection_matrix;
    }
    glm::mat4 getViewProjectionMatrix() {
        updateViewProjectionMatrix();
        return m_view_projection_matrix;
    }

    glm::vec3 getPosition() { return m_position; }

    // setter
    void tick(float delta_time, uint32_t camera_movement, float mouse_x, float mouse_y);
    void setFovY(float fovy);
    void setAspect(float aspect);

private:
    // update
    void updateVector();
    void updateViewMatrix();
    void updateProjectionMatrix();
    void updateViewProjectionMatrix();

    void onResize(int width, int height);

private:
    static const glm::vec3 UP_IN_WORLD;
    static const float     EPS;
    // fundamental info
    glm::vec3 m_position;
    float     m_yaw;
    float     m_pitch;
    // derived info
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_forward;
    // options
    float m_fovy;
    float m_aspect;
    float m_near;
    float m_far;
    float m_move_speed;
    float m_mouse_sensitivity;
    // matrices
    bool      m_view_matrix_dirty{true};
    glm::mat4 m_view_matrix{1.0f};
    bool      m_projection_matrix_dirty{true};
    glm::mat4 m_projection_matrix{1.0f};
    bool      m_view_projection_matrix_dirty{true};
    glm::mat4 m_view_projection_matrix{1.0f};
};

} // namespace BJTUGE
