#pragma once

#include "runtime/function/framework/framework_common.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace BJTUGE {

class GObject;

class Level {

public:
    Level()  = default;
    ~Level() = default;

    void initialize();

    void tick(float delta_time);

private:
    std::unordered_map<std::string, GObjectID> m_objects;
};

} // namespace BJTUGE