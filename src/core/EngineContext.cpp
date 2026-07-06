#include "EngineContext.h"

#include "../renderer/RendererManager.h"
#include "../resources/AssetManager.h"
#include "../scene/SceneManager.h"

namespace Engine {

EngineContext::EngineContext() = default;
EngineContext::~EngineContext() = default;

void EngineContext::init(EngineState &state) {
    assetManager = std::make_unique<AssetManager>(*this);
    sceneManager = std::make_unique<SceneManager>(*this, state);
    rendererManager = std::make_unique<RendererManager>(*this, state);
}

} // namespace Engine
