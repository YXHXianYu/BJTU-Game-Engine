#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace BJTUGE {

class RenderShader;
class RenderResource;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;

    Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texcoord)
        : position(position), normal(normal), texcoord(texcoord) {}
    Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
        : position(x, y, z), normal(nx, ny, nz), texcoord(u, v) {}
};

class RenderMeshBase {

public:
    RenderMeshBase()                                 = default;
    virtual ~RenderMeshBase()                        = default;
    RenderMeshBase(const RenderMeshBase&)            = delete;
    RenderMeshBase& operator=(const RenderMeshBase&) = delete;

    virtual void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) = 0;
};

} // namespace BJTUGE
