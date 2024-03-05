#pragma once

#include <string>
#include <unordered_map>

#include "runtime/function/render/render_entity.h"

namespace BJTUGE {

class RenderResource {

public:
    void initialize();
    void clear();

    std::unordered_map<std::string, RenderEntity>::iterator begin() { return m_render_entities_map.begin(); }

    std::unordered_map<std::string, RenderEntity>::iterator end() { return m_render_entities_map.end(); }

    std::unordered_map<std::string, RenderEntity>::const_iterator cbegin() const { return m_render_entities_map.cbegin(); }

    std::unordered_map<std::string, RenderEntity>::const_iterator cend() const { return m_render_entities_map.cend(); }

private:
    std::unordered_map<std::string, RenderEntity> m_render_entities_map;
};

} // namespace BJTUGE