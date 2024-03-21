#pragma once

#include "runtime/function/swap/swap_event.h"

#include <memory>
#include <queue>

namespace BJTUGE {

class SwapContext {

public:
    void initialize();
    void clear();

    bool                       isEmpty();
    void                       push(std::unique_ptr<SwapEvent> event);
    std::unique_ptr<SwapEvent> pop();

private:
    std::queue<std::unique_ptr<SwapEvent>> m_events;
};

} // namespace BJTUGE