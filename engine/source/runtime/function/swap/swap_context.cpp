#include "runtime/function/swap/swap_context.h"

namespace BJTUGE {

void SwapContext::initialize() {}

void SwapContext::clear() {}

bool SwapContext::isEmpty() {
    return m_events.empty();
}

void SwapContext::push(std::unique_ptr<SwapEvent> event) {
    m_events.push(std::move(event));
}

std::unique_ptr<SwapEvent> SwapContext::pop() {
    auto event = std::move(m_events.front());
    m_events.pop();
    return std::move(event);
}

} // namespace BJTUGE