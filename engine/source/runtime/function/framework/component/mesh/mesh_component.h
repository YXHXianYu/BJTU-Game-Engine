#pragma once

#include "runtime/function/framework/component/component.h"

namespace BJTUGE {

class MeshComponent : public Component {

public:
    MeshComponent()           = default;
    ~MeshComponent() override = 0; // TODO: Implement MeshComponent

    void initialize(std::weak_ptr<GObject> parent_object) override;

    void tick(float delta_time) override;
};

} // namespace BJTUGE