#include "runtime/function/render/render_system.h"

#include <iostream>

namespace BJTUGE {

void RenderSystem::initialize() { std::cout << "RenderSystem is initializing" << std::endl; }

void RenderSystem::tick(float delta_time) {}

void RenderSystem::clear() {}

} // namespace BJTUGE