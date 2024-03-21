#include "runtime/function/swap/minecraft_blocks/remove_block_swap_event.h"

#include "runtime/function/render/minecraft_blocks/render_minecraft_blocks_manager.h"
#include "runtime/function/render/render_resource.h"

namespace BJTUGE {

void RemoveBlockSwapEvent::update(RenderResource* render_resource) {
    render_resource->getRenderMinecraftBlocksManager()->remove(m_pos, m_face);
}

} // namespace BJTUGE