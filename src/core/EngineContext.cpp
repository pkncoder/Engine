#include "EngineContext.h"

#include "../renderer/RendererManager.h"
#include "../resources/AssetManager.h"
#include "../scene/SceneManager.h"

namespace Engine {

// Default these, as the constructor won't be used
EngineContext::EngineContext() = default;
EngineContext::~EngineContext() = default;

// Init managers
void EngineContext::init(EngineState &state) {
    assetManager = std::make_unique<AssetManager>(*this);
    sceneManager = std::make_unique<SceneManager>(*this, state);
    rendererManager = std::make_unique<RendererManager>(*this, state);
}

} // namespace Engine
