#include "core/Application.h"

int main() {
  auto app = std::make_unique<Engine::Application>();
  app->Init();
  app->Run();
  return 0;
}
