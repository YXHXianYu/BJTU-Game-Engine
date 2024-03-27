#pragma once

namespace BJTUGE {

class RenderResource;

class SwapEvent {

public:
    SwapEvent()          = default;
    virtual ~SwapEvent() = default;

    // use raw pointer for extreme performance
    virtual void update(RenderResource* render_resource) = 0;
};

} // namespace BJTUGE