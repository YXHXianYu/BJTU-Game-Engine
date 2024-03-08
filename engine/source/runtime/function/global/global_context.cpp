#include "global_context.h"

#include "runtime/function/window/window_system.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/input/input_system.h"

namespace BJTUGE {

RuntimeGlobalContext g_runtime_global_context;

void RuntimeGlobalContext::initialize() {
    m_window_system = std::make_shared<WindowSystem>();
    m_window_system->initialize(WindowCreateInfo{});

    m_render_system = std::make_shared<RenderSystem>();
    m_render_system->initialize();

    m_input_system = std::make_shared<InputSystem>();
    m_input_system->initialize();
}

void RuntimeGlobalContext::clear() {
    m_window_system->clear();
    m_window_system.reset();

    m_render_system->clear();
    m_render_system.reset();

    m_input_system->clear();
    m_input_system.reset();
}

} // namespace BJTUGE