#pragma once

#include "RenderPass.h"

#include <vector>

namespace Engine {

class RenderGraph {

  public:
    RenderGraph() = default;
    inline ~RenderGraph() { clear(); };

    void addPass(const RenderPass &pass);

    void compile();
    void execute();

    void clear();

  private:
    std::vector<RenderPass> renderPasses;
};

} // namespace Engine
