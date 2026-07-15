#pragma once

#include "../core/states/EngineState.h"

namespace Engine {

class IRenderer {
  public:
    virtual ~IRenderer() = default; // Deconstructor

    // TODO: bring back for lazyloading
    inline void init(){}; // Initializing a renderer

    virtual void render(EngineState &state) = 0; // Rendering a frame

    virtual void shutdown() = 0; // Shutting down a renderer

    virtual inline void resize(const int width, const int height) {}
    virtual inline void present(const int width, const int height) const {}
};

}; // namespace Engine
