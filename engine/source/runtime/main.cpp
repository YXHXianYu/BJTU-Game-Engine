#include "runtime/engine.h"

#include <iostream>
#include <memory>

int main() {

    std::unique_ptr<BJTUGE::BJTUEngine> engine = std::make_unique<BJTUGE::BJTUEngine>();

    std::cout << "Starting engine." << std::endl;

    engine->startEngine();

    std::cout << "Engine is started." << std::endl;
    std::cout << "Engine is running." << std::endl;

    engine->run();

    std::cout << "Shuting down engine." << std::endl;

    engine->shutdownEngine();

    std::cout << "Engine is shut down." << std::endl;

    return 0;
}