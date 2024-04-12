#include "runtime/function/input/input_system.h"

#include "runtime/function/global/global_context.h"
#include "runtime/function/window/window_system.h"
#include <cassert>

namespace BJTUGE {

void InputSystem::initialize() {
    // 在这里设置window_system的回调函数
    std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
    assert(window_system);

    window_system->registerOnKeyFunc(
        std::bind(&InputSystem::onKey, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    window_system->registerOnCursorPosFunc(std::bind(&InputSystem::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
    window_system->registerOnMouseButtonFunc(
        std::bind(&InputSystem::onMouseButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
void InputSystem::tick() {}
void InputSystem::clear() {
    m_cursor_delta_x = 0;
    m_cursor_delta_y = 0;
}

void InputSystem::onKey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_W: {
                m_game_command |= (uint32_t)GameCommand::FORWARD;
                break;
            }
            case GLFW_KEY_S: {
                m_game_command |= (uint32_t)GameCommand::BACKWARD;
                break;
            }
            case GLFW_KEY_A: {
                m_game_command |= (uint32_t)GameCommand::LEFT;
                break;
            }
            case GLFW_KEY_D: {
                m_game_command |= (uint32_t)GameCommand::RIGHT;
                break;
            }
            case GLFW_KEY_Q: {
                m_game_command |= (uint32_t)GameCommand::UP;
                break;
            }
            case GLFW_KEY_E: {
                m_game_command |= (uint32_t)GameCommand::DOWN;
                break;
            }
            case GLFW_KEY_SPACE: {
                m_game_command |= (uint32_t)GameCommand::KB_SPACE;
                break;
            }
            case GLFW_KEY_J: {
                if (mods & GLFW_MOD_SHIFT) { m_game_command ^= (uint32_t)GameCommand::RENDER_BLOCK; }
                break;
            }
            case GLFW_KEY_K: {
                if (mods & GLFW_MOD_SHIFT) { m_game_command ^= (uint32_t)GameCommand::RENDER_CHARACTER; }
                break;
            }
            case GLFW_KEY_L: {
                if (mods & GLFW_MOD_SHIFT) { m_game_command ^= (uint32_t)GameCommand::RENDER_LIGHT; }
                break;
            }
            case GLFW_KEY_O: {
                if (mods & GLFW_MOD_SHIFT) { m_game_command ^= (uint32_t)GameCommand::USE_ORTHO; }
                break;
            }
            default: {
                break;
            }
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W: {
                m_game_command &= ~(uint32_t)GameCommand::FORWARD;
                break;
            }
            case GLFW_KEY_S: {
                m_game_command &= ~(uint32_t)GameCommand::BACKWARD;
                break;
            }
            case GLFW_KEY_A: {
                m_game_command &= ~(uint32_t)GameCommand::LEFT;
                break;
            }
            case GLFW_KEY_D: {
                m_game_command &= ~(uint32_t)GameCommand::RIGHT;
                break;
            }
            case GLFW_KEY_Q: {
                m_game_command &= ~(uint32_t)GameCommand::UP;
                break;
            }
            case GLFW_KEY_E: {
                m_game_command &= ~(uint32_t)GameCommand::DOWN;
                break;
            }
            case GLFW_KEY_SPACE: {
                m_game_command &= ~(uint32_t)GameCommand::KB_SPACE;
                break;
            }
            default: {
                break;
            }
        }
    }
}

void InputSystem::onCursorPos(double xpos, double ypos) {
    if (m_game_command & static_cast<uint32_t>(GameCommand::MOUSE_RIGHT)) {
        m_cursor_delta_x = static_cast<int32_t>(xpos - m_last_cursor_x);
        m_cursor_delta_y = static_cast<int32_t>(ypos - m_last_cursor_y);
    }
    m_last_cursor_x = static_cast<uint32_t>(xpos);
    m_last_cursor_y = static_cast<uint32_t>(ypos);
}

void InputSystem::onMouseButton(int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT: {
                m_game_command |= (uint32_t)GameCommand::MOUSE_LEFT;
                break;
            }
            case GLFW_MOUSE_BUTTON_RIGHT: {
                m_game_command |= (uint32_t)GameCommand::MOUSE_RIGHT;
                break;
            }
            case GLFW_MOUSE_BUTTON_MIDDLE: {
                m_game_command |= (uint32_t)GameCommand::MOUSE_MIDDLE;
                break;
            }
            default: {
                break;
            }
        }
    } else if (action == GLFW_RELEASE) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT: {
                m_game_command &= ~(uint32_t)GameCommand::MOUSE_LEFT;
                break;
            }
            case GLFW_MOUSE_BUTTON_RIGHT: {
                m_game_command &= ~(uint32_t)GameCommand::MOUSE_RIGHT;
                break;
            }
            case GLFW_MOUSE_BUTTON_MIDDLE: {
                m_game_command &= ~(uint32_t)GameCommand::MOUSE_MIDDLE;
                break;
            }
            default: {
                break;
            }
        }
    }
}

} // namespace BJTUGE