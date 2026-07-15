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
    static bool
    isKeyPressed(const int keycode); // Is the key down at all right now?
    static bool
    isKeyJustPressed(const int keycode); // Did the key go down THIS frame?
    static bool isKeyHeld(
        const int keycode); // Is the key being held down (down for > 1 frame)?
    static bool
    isKeyLetGo(const int keycode); // Did the key get released THIS frame?

    // --- MOUSE BUTTON QUERIES ---
    static bool isMouseButtonPressed(const int button);
    static bool isMouseButtonJustPressed(const int button);
    static bool isButtonHeld(const int button);
    static bool isMouseButtonLetGo(const int button);

    // --- MOUSE MOVEMENT ---
    static glm::vec2 getMousePosition();
    inline static glm::vec2 getMouseDelta() { return mouseDelta; };

  private:
    static inline std::function<void(std::shared_ptr<IEvent>)> dispatchEvent =
        nullptr;

    static void keyEventCallback(GLFWwindow *window, const int key,
                                 const int scancode, const int action,
                                 const int mods);
    static void mouseButtonEventCallback(GLFWwindow *window, const int button,
                                         const int action, const int mods);
    static void cursorEventCallback(GLFWwindow *window, const double xPos,
                                    const double yPos);
    static void scrollEventCallback(GLFWwindow *window, const double xOffset,
                                    const double yOffset);

  private:
    static inline GLFWwindow *window = nullptr;
    static inline EngineState *engineState = nullptr;

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
