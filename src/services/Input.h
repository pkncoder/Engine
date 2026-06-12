#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Engine {

class Input {
  public:
    // Initialize the input "serivce"
    static void init(GLFWwindow *window);

    // Update information per-frame
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
    static inline GLFWwindow *window = nullptr;

    // Mouse movement
    static inline glm::vec2 lastMousePos = {0.0, 0.0};
    static inline glm::vec2 mouseDelta = {0.0, 0.0};
};

} // namespace Engine
