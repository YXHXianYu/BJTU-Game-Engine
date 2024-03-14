#include "runtime/function/render/render_face_manager.h"

namespace BJTUGE {

const uint32_t RenderFaceManager::k_buffer_num  = 1024;
const uint32_t RenderFaceManager::k_buffer_size = k_buffer_num * sizeof(FaceInfo);
const uint32_t RenderFaceManager::k_cache_num   = k_buffer_num;

void RenderFaceManager::initialize() {}
void RenderFaceManager::clear() {}

} // namespace BJTUGE