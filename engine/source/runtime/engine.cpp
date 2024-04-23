#include "runtime/engine.h"

#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/window/window_system.h"

#include "runtime/core/utils.h"

#include <iostream>
#include <cassert>

namespace BJTUGE {

void BJTUEngine::startEngine() {
    g_runtime_global_context.initialize();
}

void BJTUEngine::shutdownEngine() {
    g_runtime_global_context.clear();
}

void BJTUEngine::run() {
    uint64_t fps_count = 0;
    uint64_t fps_delta = 0;
    uint64_t fps_patch = 20;
    
    while (!g_runtime_global_context.m_window_system->windowShouldClose()) {

        // delta time
        g_runtime_global_context.last_time = g_runtime_global_context.cur_time;
        g_runtime_global_context.cur_time = getCurrentTimeMillis();
        uint64_t delta = g_runtime_global_context.cur_time - g_runtime_global_context.last_time;

        // fps
        if (delta > 0) {
            fps_delta += delta;
            if (++fps_count >= fps_patch) {
                std::cout << "FPS: " << uint64_t(fps_patch * 1000.0 / fps_delta) << std::endl;
                fps_count = 0;
                fps_delta = 0;
            }
        }

        // tick
        tick(delta);

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
