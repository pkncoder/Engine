#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Engine {

// TODO: temp
struct TempVertex {
    glm::vec3 position;
    glm::vec3 color;
};

class BufferManager {
  public:
    static uint32_t
    createSimpleMesh(const std::vector<TempVertex> &vertices); // TODO: temp
};

} // namespace Engine
