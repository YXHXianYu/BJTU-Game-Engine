#pragma once

#include <cstdint>

namespace BJTUGE {

enum class GameCommand : uint32_t {
    FORWARD          = 1U << 0,
    BACKWARD         = 1U << 1,
    LEFT             = 1U << 2,
    RIGHT            = 1U << 3,
    UP               = 1U << 4,
    DOWN             = 1U << 5,
    MOUSE_LEFT       = 1U << 6,
    MOUSE_RIGHT      = 1U << 7,
    MOUSE_MIDDLE     = 1U << 8,
    KB_SPACE         = 1U << 9,
    RENDER_BLOCK     = 1U << 10,
    RENDER_CHARACTER = 1U << 11,
    RENDER_LIGHT     = 1U << 12,
    USE_ORTHO        = 1U << 13,

    INVALID = 1U << 31,
};

class InputSystem {

public:
    void initialize();
    void tick();
    void clear();

    void     resetGameCommand() { m_game_command = 0; }
    uint32_t getGameCommand() const { return m_game_command; }
    int32_t  getCursorDeltaX() const { return m_cursor_delta_x; }
    int32_t  getCursorDeltaY() const { return m_cursor_delta_y; }
    void     resetCursorDelta() {
        m_cursor_delta_x = 0;
        m_cursor_delta_y = 0;
    }

private:
    void onKey(int key, int scancode, int action, int mods);
    void onCursorPos(double xpos, double ypos);
    void onMouseButton(int button, int action, int mods);

private:
    uint32_t m_game_command{0};

    uint32_t m_last_cursor_x{0};
    uint32_t m_last_cursor_y{0};

    int32_t m_cursor_delta_x{0};
    int32_t m_cursor_delta_y{0};
};

} // namespace BJTUGE