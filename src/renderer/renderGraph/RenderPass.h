#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

using ResourceHandle = std::uint64_t;

namespace Engine {

struct RenderPass {

  public:
    // Public functions set by lambdas
    std::function<void()> setup;
    std::function<void()> execute;

  public:
    // Identifier for UI
    std::string name;

  private:
    // Shaders
    ResourceHandle addProgram();
    ResourceHandle addShader();

    // Storage
    ResourceHandle addBuffer();

    // Textures
    ResourceHandle addRenderTarget();
    ResourceHandle addTexture();

  private:
    // Program
    ResourceHandle program;

    // Links to resource handles
    std::vector<ResourceHandle> inputs;
    std::vector<ResourceHandle> outputs;
};

} // namespace Engine
