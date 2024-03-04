#pragma once

namespace BJTUGE {

class BJTUEngine {
public:
    void startEngine();
    void shutdownEngine();

    void run();
    void tick(float delta_time);

private:
    void tickLogic(float delta_time);
    void tickRender(float delta_time);
};

} // namespace BJTUGE