#pragma once

#include "runtime/function/render/mesh/face_info.h"
#include "runtime/function/swap/swap_event.h"

namespace BJTUGE {

struct FaceInfo;
class RenderResource;

class AddBlockSwapEvent : public SwapEvent {

public:
    AddBlockSwapEvent(const FaceInfo& face) : m_face(face) {}
    ~AddBlockSwapEvent() override {}

    void update(RenderResource* render_resource) override;

private:
    FaceInfo m_face;
};

} // namespace BJTUGE