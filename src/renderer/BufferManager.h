#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Color;
};

class BufferManager {
public:
  // Returns the VAO ID
  static uint32_t CreateSimpleMesh(const std::vector<Vertex> &vertices);
};
