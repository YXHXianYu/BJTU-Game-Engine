#include "runtime/function/swap/minecraft_blocks/add_block_swap_event.h"

#include "runtime/function/render/minecraft_blocks/render_minecraft_blocks_manager.h"
#include "runtime/function/render/render_resource.h"

namespace BJTUGE {

void AddBlockSwapEvent::update(RenderResource* render_resource) {
    render_resource->getRenderMinecraftBlocksManager()->insert(m_face);
}

} // namespace BJTUGE