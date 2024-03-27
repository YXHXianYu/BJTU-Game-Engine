#include "runtime/function/render/render_camera.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/window/window_system.h"
#include "runtime/function/input/input_system.h"

#include <glm/gtc/matrix_transform.hpp>

namespace BJTUGE {

const glm::vec3 RenderCamera::UP_IN_WORLD{0.0f, 1.0f, 0.0f};
const float     RenderCamera::EPS = 1e-3f;

RenderCamera::RenderCamera(CameraCreateInfo info)
    : m_position(info.position), m_yaw(info.yaw), m_pitch(info.pitch), m_fovy(info.fovy), m_aspect(info.aspect), m_near(info.near),
      m_far(info.far), m_ortho_left{info.left}, m_ortho_right{info.right}, m_ortho_top{info.top}, m_ortho_buttom{info.buttom},
    m_move_speed(info.move_speed), m_mouse_sensitivity(info.mouse_sensitivity) {
    updateVector();

    // window resize
    g_runtime_global_context.m_window_system->registerOnResizeFunc(
        std::bind(&RenderCamera::onResize, this, std::placeholders::_1, std::placeholders::_2));
}

// === setter ===
void RenderCamera::tick(float delta_time, uint32_t camera_movement, float mouse_x, float mouse_y) {
    if (camera_movement != 0 && !(camera_movement & static_cast<uint32_t>(GameCommand::INVALID))) {
        float velocity = m_move_speed * delta_time;
        if (camera_movement & static_cast<uint32_t>(GameCommand::FORWARD)) m_position += m_forward * velocity;
        if (camera_movement & static_cast<uint32_t>(GameCommand::BACKWARD)) m_position -= m_forward * velocity;
        if (camera_movement & static_cast<uint32_t>(GameCommand::LEFT)) m_position -= m_right * velocity;
        if (camera_movement & static_cast<uint32_t>(GameCommand::RIGHT)) m_position += m_right * velocity;
        if (camera_movement & static_cast<uint32_t>(GameCommand::UP)) m_position += UP_IN_WORLD * velocity;
        if (camera_movement & static_cast<uint32_t>(GameCommand::DOWN)) m_position -= UP_IN_WORLD * velocity;
        m_view_matrix_dirty = true;
    }

    if (std::abs(mouse_x) > EPS || std::abs(mouse_y) > EPS) {
        m_yaw += mouse_x * m_mouse_sensitivity;
        m_pitch += -1.0 * mouse_y * m_mouse_sensitivity;
        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
        updateVector();
    }
}
void RenderCamera::setFovY(float fovy) {
    m_fovy                    = fovy;
    m_projection_matrix_dirty = true;
}
void RenderCamera::setAspect(float aspect) {
    m_aspect                  = aspect;
    m_projection_matrix_dirty = true;
}

// === update (private) ===
void RenderCamera::updateVector() {
    m_front   = glm::vec3{cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), sin(glm::radians(m_pitch)),
                        sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))};
    m_front   = glm::normalize(m_front);
    m_right   = glm::normalize(glm::cross(m_front, UP_IN_WORLD));
    m_up      = glm::normalize(glm::cross(m_right, m_front));
    m_forward = glm::normalize(glm::cross(UP_IN_WORLD, m_right));

    m_view_matrix_dirty = true;
}

void RenderCamera::updateViewMatrix() {
    if (m_view_matrix_dirty) {
        m_view_matrix                  = glm::lookAt(m_position, m_position + m_front, m_up);
        m_view_matrix_dirty            = false;
        m_view_projection_matrix_dirty = true;
    }
}

void RenderCamera::updateProjectionMatrix(bool use_ortho) {
    if (m_projection_matrix_dirty) {
        if (use_ortho) {
            m_projection_matrix = glm::ortho(m_ortho_left, m_ortho_right, m_ortho_buttom, m_ortho_top, m_near, m_far);
        } else {
            m_projection_matrix = glm::perspective(glm::radians(m_fovy), m_aspect, m_near, m_far);
        }
        m_projection_matrix_dirty      = false;
        m_view_projection_matrix_dirty = true;
    }
}

void RenderCamera::updateViewProjectionMatrix(bool use_ortho) {
    updateViewMatrix();
    updateProjectionMatrix(use_ortho);
    if (m_view_projection_matrix_dirty) {
        m_view_projection_matrix       = m_projection_matrix * m_view_matrix;
        m_view_projection_matrix_dirty = false;
    }
}

void RenderCamera::onResize(int width, int height) {
    setAspect(static_cast<float>(width) / static_cast<float>(height));
}

} // namespace BJTUGE
