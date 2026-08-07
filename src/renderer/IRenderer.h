#pragma once

#include "../core/states/EngineState.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"

namespace Engine {

struct RenderPacket {
    glm::mat4 modelMatrix;
    AssetHandle meshHandle;
    AssetHandle materialHandle;
};

class IRenderer {
  public:
    virtual ~IRenderer() = default; // Deconstructor

    // --- Lifecycle ---

    virtual void init(EngineState &state) = 0;
    virtual void shutdown() = 0;
    virtual void resize(const uint32_t width, const uint32_t height) = 0;

    // --- The Frame Pipeline ---

    virtual void beginFrame(EngineState &state) = 0;

    virtual void extract(EngineState &state) = 0;
    virtual void prepare(EngineState &state) = 0;

    virtual void dispatch(EngineState &state) = 0;
    virtual void postProcess(EngineState &state) = 0;

    virtual void present(EngineState &state) = 0;
};

}; // namespace Engine
