#pragma once

#include "IEvent.h"

#include <memory>

namespace Engine {

// Event dispatcher base class
class IEventDispatcher {
  public:
    virtual ~IEventDispatcher() = default;
    virtual void dispatchEvent(std::shared_ptr<IEvent> event) const = 0;
};

} // namespace Engine
