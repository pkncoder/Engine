#pragma once

#include "RenderPass.h"

#include <vector>

namespace Engine {

class RenderGraph {

  public:
    RenderGraph() = default;
    inline ~RenderGraph() { clear(); };

    void addPass(const RenderPass &pass);

    void setup();
    void compile();
    void execute();

    void clear();

  public:
    RenderPass *graphStart;
    ResourceHandle graphEnd;

  private:
    std::vector<RenderPass> renderPasses;
};

} // namespace Engine
