#pragma once

#include <iostream>
#include <memory>

namespace BJTUGE {

class RenderShader;

class RenderTextureBase {
public:
    RenderTextureBase()                                    = default;
    virtual ~RenderTextureBase()                           = default;
    RenderTextureBase(const RenderTextureBase&)            = delete;
    RenderTextureBase& operator=(const RenderTextureBase&) = delete;

    virtual void use(std::shared_ptr<RenderShader> shader, const std::string& name, uint32_t texture_id) const = 0;
};

} // namespace BJTUGE