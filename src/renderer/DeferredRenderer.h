#pragma once

#include "../core/EngineContext.h"
#include "../core/states/EngineState.h"
#include "GPUStructs.h"
#include "IRenderer.h"
#include "RenderGraph.h"
#include "buffers/GPUBuffer.h"
#include "shaders/IProgram.h"

#include <vector>

namespace Engine {

class DeferredRenderer : public IRenderer {

  public:
    inline DeferredRenderer(EngineContext &_engineContext)
        : engineContext(_engineContext) {}
    inline ~DeferredRenderer() { shutdown(); }

    void init(EngineState &state) override;
    void shutdown() override;
    void resize(const uint32_t width, const uint32_t height) override;

    void beginFrame(EngineState &state) override;
    void extract(EngineState &state) override;
    void prepare(EngineState &state) override;
    void dispatch(EngineState &state) override;
    void present(EngineState &state) override;

  private:
    EngineContext &engineContext;

    uint32_t frameIndex = 0;

    std::vector<RasterDrawCommand> opaqueCommands;
    std::vector<PointLightData> pointLights;

    CameraData cameraData;

    BufferHandle cameraUBO;
    BufferHandle lightUBO;

    RenderTargetHandle gBufferHandle = INVALID_RENDER_TARGET;
    RenderTargetHandle finalOutputHandle = INVALID_RENDER_TARGET;

    IProgram gBufferProgram;
    IProgram lightingProgram;

    RenderGraph renderGraph;
};

} // namespace Engine
