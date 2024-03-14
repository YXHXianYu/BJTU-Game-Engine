#pragma once

#include <memory>

namespace BJTUGE {

class WindowSystem;
class RenderSystem;
class InputSystem;

class WorldManager;

class RuntimeGlobalContext {

public:
    void initialize();
    void clear();

    std::shared_ptr<WindowSystem> m_window_system;
    std::shared_ptr<RenderSystem> m_render_system;
    std::shared_ptr<InputSystem>  m_input_system;

    std::shared_ptr<WorldManager> m_world_manager;
};

extern RuntimeGlobalContext g_runtime_global_context;

} // namespace BJTUGE