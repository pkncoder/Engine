#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input {
  public:
    // Initialize the input "serivce"
    static void Init(GLFWwindow *window);

    // Polling input
    static void Update();

    // Check for a keypress
    static bool IsKeyPressed(int keycode);

    // Check for a button press
    static bool IsMouseButtonPressed(int button);

    // Getting mouse movement
    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseDelta();

  private:
    // Window pointer
    static GLFWwindow *s_Window;

    // Mouse movement
    static glm::vec2 s_LastMousePos;
    static glm::vec2 s_MouseDelta;
};
