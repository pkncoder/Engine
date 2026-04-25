#pragma once
#include "../renderer/Rasterizer.h"
#include "../scene/camera.h"
#include "Window.h"
#include <memory>

namespace Engine {
class Application {
public:
  Application();
  ~Application();

  void Run();
  void Init();

private:
  std::unique_ptr<Window> m_Window;
  Camera m_Camera;
  std::unique_ptr<Rasterizer> m_Rasterizer;
  bool m_Running = true;
};
} // namespace Engine
