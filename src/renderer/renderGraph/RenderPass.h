#pragma once

#include <functional>
#include <string>

namespace Engine {

struct RenderPass {
    std::string name;

    std::function<void()> setup;

    std::function<void()> execute;
};

} // namespace Engine
