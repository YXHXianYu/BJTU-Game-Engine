#pragma once

#include "runtime/function/swap/swap_event.h"

#include <glm/glm.hpp>

namespace BJTUGE {

struct FaceInfo;
class RenderResource;

class RemoveBlockSwapEvent : public SwapEvent {

public:
    RemoveBlockSwapEvent(const glm::vec3& pos, float face) : m_pos(pos), m_face(face) {}
    ~RemoveBlockSwapEvent() override {}

    void update(RenderResource* render_resource) override;

private:
    glm::vec3 m_pos;
    float     m_face;
};

} // namespace BJTUGE