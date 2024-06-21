#pragma once

#include <glm/glm.hpp>

namespace BJTUGE {

struct RenderMaterial {
    glm::vec3 albedo;
    float     metallic;
    float     roughness;
    float     ao;

    RenderMaterial() : albedo(1.0f), metallic(0.0f), roughness(0.5f), ao(1.0f) {}
    RenderMaterial(const glm::vec3& albedo, float metallic, float roughness, float ao)
        : albedo(albedo), metallic(metallic), roughness(roughness), ao(ao) {}
};

} // namespace BJTUGE