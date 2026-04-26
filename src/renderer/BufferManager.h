#pragma once

#include <glm/glm.hpp>
#include <vector>

// TODO: Here?
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Color;
};

class BufferManager {
  public:
    static uint32_t CreateSimpleMesh(const std::vector<Vertex> &vertices);
};
