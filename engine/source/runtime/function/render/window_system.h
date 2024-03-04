#pragma once

namespace BJTUGE {

class WindowSystem {
public:
    void initialize();
    void tick(float delta_time);
    void clear();
};

} // namespace BJTUGE