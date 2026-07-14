#pragma once

#include "../core/events/IEvent.h"
#include "../core/states/EngineState.h"

#include <GLFW/glfw3.h>
#include <array>
#include <glm/glm.hpp>

#include <memory>

namespace Engine {

class Input {
  public:
    static void init(GLFWwindow *window);
    static inline void setEventCallback(
        const std::function<void(std::shared_ptr<IEvent>)> &callback) {
        dispatchEvent = callback;
    };

    static void update();

    // --- KEYBOARD QUERIES ---
    static bool isKeyPressed(int keycode); // Is the key down at all right now?
    static bool
    isKeyJustPressed(int keycode); // Did the key go down THIS frame?
    static bool
    isKeyHeld(int keycode); // Is the key being held down (down for > 1 frame)?
    static bool isKeyLetGo(int keycode); // Did the key get released THIS frame?

    // --- MOUSE BUTTON QUERIES ---
    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonJustPressed(int button);
    static bool isButtonHeld(int button);
    static bool isMouseButtonLetGo(int button);

    // --- MOUSE MOVEMENT ---
    static glm::vec2 getMousePosition();
    inline static glm::vec2 getMouseDelta() { return mouseDelta; };

  private:
    static inline GLFWwindow *window = nullptr;
    static inline EngineState *engineState = nullptr;

    static inline std::function<void(std::shared_ptr<IEvent>)> dispatchEvent =
        nullptr;

    static void keyEventCallback(GLFWwindow *window, int key, int scancode,
                                 int action, int mods);
    static void mouseButtonEventCallback(GLFWwindow *window, int button,
                                         int action, int mods);
    static void cursorEventCallback(GLFWwindow *window, double xPos,
                                    double yPos);
    static void scrollEventCallback(GLFWwindow *window, double xOffset,
                                    double yOffset);
    static void framebufferSizeEventCallback(GLFWwindow *window, int width,
                                             int height);

    // Mouse metrics
    static inline glm::vec2 lastMousePos = {0.0f, 0.0f};
    static inline glm::vec2 mouseDelta = {0.0f, 0.0f};

    // Keyboard State Tables
    static inline std::array<bool, GLFW_KEY_LAST + 1> keysRealtime{};
    static inline std::array<bool, GLFW_KEY_LAST + 1> keysCurrent{};
    static inline std::array<bool, GLFW_KEY_LAST + 1> keysPrevious{};

    // Mouse Button State Tables
    static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>
        buttonsRealtime{};
    static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> buttonsCurrent{};
    static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>
        buttonsPrevious{};
};

} // namespace Engine
