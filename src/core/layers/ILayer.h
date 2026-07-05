#pragma once

#include "../states/EngineState.h"

namespace Engine {

class ILayer {
  public:
    virtual ~ILayer() = default;

    virtual void onAttach(EngineState &state) = 0;
    virtual void onUpdate(EngineState &state) = 0;
    virtual void onDetach() = 0;
};

} // namespace Engine
