#include "runtime/engine.h"

#include "runtime/function/global/global_context.h"

namespace BJTUGE {

void BJTUEngine::startEngine() { g_runtime_global_context.initialize(); }

void BJTUEngine::shutdownEngine() { g_runtime_global_context.clear(); }

void BJTUEngine::run() {
    while (true) {
        tick(10);
    }
}

void BJTUEngine::tick(float delta_time) {
    tickLogic(delta_time);

    tickRender(delta_time);
}

void BJTUEngine::tickLogic(float delta_time) {}

void BJTUEngine::tickRender(float delta_time) {}

} // namespace BJTUGE