#pragma once

#include <functional>
#include <string>
#include <vector>

namespace Engine {

struct RenderPass {
    std::string name;

    std::function<void()> setup;

    std::function<void()> execute;
};

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
