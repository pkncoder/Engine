#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input {
  public:
    // Initialize the input "serivce"
    static void init(GLFWwindow *window);

    // Polling input
    static void update();

    // Check for a keypress
    static bool isKeyPressed(int keycode);

    // Check for a button press
    static bool isMouseButtonPressed(int button);

    // Getting mouse movement
    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();

  private:
    // Window pointer
    static GLFWwindow *window;

    // Mouse movement
    static glm::vec2 lastMousePos;
    static glm::vec2 mouseDelta;
};
