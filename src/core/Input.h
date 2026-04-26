#pragma once

#include <glm/glm.hpp>

struct GLFWwindow; // Forward declaration

class Input {
  public:
    // Initialize with the window context
    static void Init(GLFWwindow *window);

    // Called once per frame at the start of the loop
    static void Update();

    // Keyboard polling
    static bool IsKeyPressed(int keycode);

    // Mouse polling
    static bool IsMouseButtonPressed(int button);

    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseDelta();

  private:
    static GLFWwindow *s_Window;
    static glm::vec2 s_LastMousePos;
    static glm::vec2 s_MouseDelta;
};
