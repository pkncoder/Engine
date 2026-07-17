#pragma once

#include "RendererState.h"
#include "SceneState.h"
#include "WindowState.h"

namespace Engine {

struct EngineState {
  public:
    // State collections
    WindowState window = WindowState();
    RendererState renderer = RendererState();
    SceneState scene = SceneState();
};

} // namespace Engine
