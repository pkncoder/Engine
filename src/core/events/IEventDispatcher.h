#pragma once

#include "IEvent.h"

#include <memory>

namespace Engine {

class IEventDispatcher {
  public:
    virtual void dispatchEvent(std::shared_ptr<IEvent> event) const = 0;
    virtual ~IEventDispatcher() = default;
};

} // namespace Engine
