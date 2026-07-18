#pragma once

#include "../core/states/EngineState.h"

namespace Engine {

class IRenderer {
  public:
    virtual ~IRenderer() = default; // Deconstructor

    // TODO: bring back for lazyloading
    inline void init(){};        // Main initializing
    virtual void shutdown() = 0; // Data cleanup

    // Main render entry
    virtual void render(EngineState &state) = 0;

    // Update render output sizes
    virtual inline void resize(const int width, const int height) {}

    // Blit the frame if needed
    virtual inline void present(const int width, const int height) const {}
};

}; // namespace Engine
