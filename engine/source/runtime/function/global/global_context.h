#pragma once

#include <memory>
#include <cstdint>

namespace BJTUGE {

class WindowSystem;
class RenderSystem;
class InputSystem;
class WorldManager;
class SwapContext;

class RuntimeGlobalContext {

public:
    void initialize();
    void clear();

    std::shared_ptr<WindowSystem> m_window_system;
    std::shared_ptr<RenderSystem> m_render_system;
    std::shared_ptr<InputSystem>  m_input_system;
    std::shared_ptr<WorldManager> m_world_manager;
    std::shared_ptr<SwapContext>  m_swap_context;

    uint64_t last_time;
    uint64_t cur_time;

    uint64_t last_tick; // TODO
    uint64_t cur_tick;
};

extern RuntimeGlobalContext g_runtime_global_context;

} // namespace BJTUGE
