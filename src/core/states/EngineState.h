#pragma once

#include "RendererState.h"
#include "SceneState.h"
#include "WindowState.h"

namespace Engine {

struct EngineState {
  public:
    WindowState windowState = WindowState();
    RendererState rendererState = RendererState();
    SceneState sceneState = SceneState();
};

} // namespace Engine
