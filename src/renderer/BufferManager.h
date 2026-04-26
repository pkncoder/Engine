#pragma once

#include <glm/glm.hpp>

#include <vector>

// TODO: Here?
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class BufferManager {
  public:
    static uint32_t createSimpleMesh(const std::vector<Vertex> &vertices);
};
