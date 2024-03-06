#include "runtime/function/render/render_pipeline.h"

#include <basic_vert.h>
#include <basic_frag.h>

namespace BJTUGE {

void RenderPipeline::initialize() {
    m_render_shaders["basic"] = std::make_shared<RenderShader>(BASIC_VERT, BASIC_FRAG);
}

void RenderPipeline::draw(std::shared_ptr<RenderResource> render_resource) {
    for (auto& [uuid, entity] : *render_resource) {

        m_render_shaders["basic"]->use();
        entity.m_render_meshes[0].use();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

} // namespace BJTUGE
