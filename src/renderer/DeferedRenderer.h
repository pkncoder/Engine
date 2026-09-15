#pragma once

#include "../core/states/EngineState.h"
#include "IRenderer.h"
#include "RenderGraph.h"

namespace Engine {

class DeferedRenderer : public IRenderer {

  public:
    void init(EngineState &state) override;
    void shutdown() override;
    void resize(const uint32_t width, const uint32_t height) override;

    void beginFrame(EngineState &state) override;
    void extract(EngineState &state) override;
    void prepare(EngineState &state) override;
    void dispatch(EngineState &state) override;
    void present(EngineState &state) override;
    void postProcess(EngineState &state) override;

  private:
    RenderGraph renderGraph;
};

} // namespace Engine
