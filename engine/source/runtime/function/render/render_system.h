#pragma once

namespace BJTUGE {

class RenderSystem {
public:
    void initialize();
    void tick(float delta_time);
    void clear();
};

} // namespace BJTUGE