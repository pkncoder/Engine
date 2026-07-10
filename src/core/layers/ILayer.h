#pragma once

#include "../events/IEvent.h"
#include "../states/EngineState.h"

#include <memory>

namespace Engine {

class ILayer {
  public:
    virtual ~ILayer() = default;

    virtual void onAttach(EngineState &state) = 0;
    virtual void onUpdate(EngineState &state) = 0;
    virtual void onDetach() = 0;
    virtual void onEvent(std::shared_ptr<IEvent> event, EngineState &state) = 0;
};

} // namespace Engine
