#pragma once

#include "runtime/function/framework/component/component.h"

namespace BJTUGE {

class MeshComponent : public Component {

public:
    void initialize(std::weak_ptr<GObject> parent_object) override;

    void tick(float delta_time) override;
};

} // namespace BJTUGE