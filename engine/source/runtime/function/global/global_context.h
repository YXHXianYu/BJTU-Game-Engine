#pragma once

#include <memory>

#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"

namespace BJTUGE {

class WindowSystem;
class RenderSystem;

class RuntimeGlobalContext {

public:
    void initialize();
    void clear();

    std::shared_ptr<WindowSystem> m_window_system;
    std::shared_ptr<RenderSystem> m_render_system;
};

extern RuntimeGlobalContext g_runtime_global_context;

} // namespace BJTUGE