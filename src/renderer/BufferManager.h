#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Engine {

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class BufferManager {
  public:
    static uint32_t
    createSimpleMesh(const std::vector<Vertex> &vertices); // TODO: temp
};

} // namespace Engine
