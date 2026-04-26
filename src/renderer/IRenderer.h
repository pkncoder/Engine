#pragma once

#include <glm/glm.hpp>

class IRenderer {
public:
  virtual ~IRenderer() = default;

  virtual void Init() = 0;
  // virtual void Shutdown() = 0;

  // We pass a reference to the camera or a FrameContext here
  virtual void Render(const glm::mat4 view, const glm::mat4 proj) = 0;
};
