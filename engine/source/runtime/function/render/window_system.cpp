#include "runtime/function/render/window_system.h"

#include <iostream>

namespace BJTUGE {

void WindowSystem::initialize() { std::cout << "RenderSystem is initializing" << std::endl; }

void WindowSystem::tick(float delta_time) {}

void WindowSystem::clear() {}

} // namespace BJTUGE