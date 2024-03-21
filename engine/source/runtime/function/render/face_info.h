#pragma once

#include <glm/glm.hpp>

namespace BJTUGE {

struct FaceInfo {
    glm::vec3 position;
    float     face;
    float     material_id; // float储存 [0, 2^23-1] 的整数是精确的，不会发生精度损失

    FaceInfo() : position(glm::vec3{0.0f}), face(0.0f), material_id(-1.0f) {} // default to create a invalid face
    FaceInfo(float x, float y, float z, float f, float id) : position(glm::vec3{x, y, z}), face(f), material_id(id) {}
    FaceInfo(int32_t x, int32_t y, int32_t z, uint32_t f, uint32_t id) : position(glm::vec3{x, y, z}), face(f), material_id(id) {}
};

} // namespace BJTUGE