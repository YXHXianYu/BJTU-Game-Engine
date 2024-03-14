#include "runtime/engine.h"

#include <cassert>

#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/window/window_system.h"

namespace BJTUGE {

void BJTUEngine::startEngine() {
    g_runtime_global_context.initialize();
}

void BJTUEngine::shutdownEngine() {
    g_runtime_global_context.clear();
}

void BJTUEngine::run() {
    while (!g_runtime_global_context.m_window_system->windowShouldClose()) {
        tick(10);
    }
}

void BJTUEngine::tick(float delta_time) {
    tickLogic(delta_time);
    tickRender(delta_time);
}

void BJTUEngine::tickLogic(float delta_time) {
    g_runtime_global_context.m_input_system->tick();

    g_runtime_global_context.m_world_manager->tick(delta_time);

    // TODO: initialize world =============================================
}

void BJTUEngine::tickRender(float delta_time) {
    g_runtime_global_context.m_window_system->tick(delta_time);
    g_runtime_global_context.m_render_system->tick(delta_time);
}

} // namespace BJTUGE