#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

namespace BJTUGE {

struct CameraCreateInfo {
    glm::vec3 position{0.0f, 1.0f, 3.0f};
    float     yaw{-90.0f};
    float     pitch{89.0f};

    float fovy{90.0f};
    float aspect{16.0f / 9.0f};
    float near{0.1f};
    float far{100.0f}; // too large far may cause precision problem

    float left{-10.0f};
    float right{10.0f};
    float top{10.0f};
    float buttom{-10.0f};

    float move_speed{0.004f};
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
    glm::mat4 getInverseViewMatrix() {
        updateViewMatrix();
        return m_inverse_view_matrix;
    }
    glm::mat4 getProjectionMatrix(bool use_ortho = false) {
        updateProjectionMatrix(use_ortho);
        return m_projection_matrix;
    }
    glm::mat4 getInverseProjectionMatrix(bool use_ortho = false) {
        updateProjectionMatrix(use_ortho);
        return m_inverse_projection_matrix;
    }
    glm::mat4 getViewProjectionMatrix(bool use_ortho = false) {
        updateViewProjectionMatrix(use_ortho);
        return m_view_projection_matrix;
    }
    glm::mat4 getInverseViewProjectionMatrix(bool use_ortho = false) {
        updateViewProjectionMatrix(use_ortho);
        return m_inverse_view_projection_matrix;
    }

    glm::vec3 getPosition() const { return m_position; }

    float getNear() const { return m_near; }
    float getFar() const { return m_far; }
    float getFovY() const { return m_fovy; }
    float getAspect() const { return m_aspect; }

    // setter
    void tick(float delta_time, uint32_t camera_movement, float mouse_x, float mouse_y);
    void setFovY(float fovy);
    void setAspect(float aspect);

private:
    // update
    void updateVector();
    void updateViewMatrix();
    void updateProjectionMatrix(bool use_ortho);
    void updateViewProjectionMatrix(bool use_ortho);

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
    // options for orthographic
    float m_ortho_left;
    float m_ortho_right;
    float m_ortho_top;
    float m_ortho_buttom;
    // matrices
    bool      m_view_matrix_dirty{true};
    glm::mat4 m_view_matrix{1.0f};
    glm::mat4 m_inverse_view_matrix{1.0f};
    bool      m_projection_matrix_dirty{true};
    glm::mat4 m_projection_matrix{1.0f};
    glm::mat4 m_inverse_projection_matrix{1.0f};
    bool      m_view_projection_matrix_dirty{true};
    glm::mat4 m_view_projection_matrix{1.0f};
    glm::mat4 m_inverse_view_projection_matrix{1.0f};

    bool current_ortho{false};
};

} // namespace BJTUGE
