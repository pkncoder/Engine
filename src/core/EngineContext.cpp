#include "EngineContext.h"

#include "../renderer/RendererManager.h"
#include "../resources/AssetManager.h"
#include "../scene/Scene.h"

namespace Engine {

void EngineContext::init(EngineState &state) {
    assetManager = std::make_unique<AssetManager>(*this);
    activeScene = std::make_unique<Scene>();
    rendererManager = std::make_unique<RendererManager>(*this, state.renderer);
}

} // namespace Engine
