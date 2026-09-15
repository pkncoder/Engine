#include "RenderGraph.h"

namespace Engine {

void RenderGraph::addPass(const RenderPass &pass) {
    renderPasses.push_back(pass);
}

void RenderGraph::compile() {
    for (auto &pass : renderPasses) {
        pass.setup();
    }
}

void RenderGraph::execute() {
    for (auto &pass : renderPasses) {
        pass.execute();
    }
}

void RenderGraph::clear() { renderPasses.clear(); }

} // namespace Engine
