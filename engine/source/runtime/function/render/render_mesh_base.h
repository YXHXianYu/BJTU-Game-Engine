#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace BJTUGE {

class RenderShader;
class RenderResource;

class RenderMeshBase {

public:
    RenderMeshBase()                                 = default;
    virtual ~RenderMeshBase()                        = default;
    RenderMeshBase(const RenderMeshBase&)            = delete;
    RenderMeshBase& operator=(const RenderMeshBase&) = delete;

    virtual void draw(std::shared_ptr<RenderShader> shader, std::shared_ptr<RenderResource> resource, glm::mat4 model) = 0;
};

} // namespace BJTUGE