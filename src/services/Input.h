#pragma once

#include "../core/events/IEvent.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace Engine {

class Input {

  public:
    // Init alizer and setter for the event callback
    static void init(GLFWwindow *window);
    static inline void setEventCallback(
        const std::function<void(std::shared_ptr<IEvent>)> &callback) {
        dispatchEvent = callback;
    };

    // Polling
    static void poll();

    // --- KEYBOARD QUERIES ---

    static bool
    isKeyPressed(const int keycode); // Is this key  currently pressed
    static bool
    isKeyJustPressed(const int keycode); // Did this key just get pressed
    static bool
    isKeyHeld(const int keycode); // Was tihs key pressed last frame also
    static bool isKeyLetGo(const int keycode); // Did this key just get released

    // --- MOUSE BUTTON QUERIES ---

    static bool
    isMouseButtonPressed(const int button); // Is this button currently pressed
    static bool isMouseButtonJustPressed(
        const int button); // Is this button get pressed this frame
    static bool
    isButtonHeld(const int button); // Was this button pressed last frame also
    static bool
    isMouseButtonLetGo(const int button); // Did this button just get released

    // --- MOUSE MOVEMENT ---

    static glm::vec2 getMousePosition(); // Position on window
    inline static glm::vec2 getMouseDelta() {
        return mouseDelta;
    }; // Change in mouse position from last frame

    // TODO: mouse acceleration (d^2x/dy^x)

  private:
    // Main event dispatcher event dispatch function
    static inline std::function<void(std::shared_ptr<IEvent>)> dispatchEvent =
        nullptr;

    // Callbacks
    static void keyEventCallback(GLFWwindow *window, const int key,
                                 const int scancode, const int action,
                                 const int mods); // Key
    static void mouseButtonEventCallback(GLFWwindow *window, const int button,
                                         const int action,
                                         const int mods); // Mouse (buttons)
    static void cursorEventCallback(GLFWwindow *window, const double xPos,
                                    const double yPos); // Mouse (cursor)
    static void scrollEventCallback(GLFWwindow *window, const double xOffset,
                                    const double yOffset); // Scroll

  private:
    // Window pointer for input state
    static inline GLFWwindow *window = nullptr;

    // Mouse metrics
    static inline glm::vec2 lastMousePos = {0.0f, 0.0f};
    static inline glm::vec2 mouseDelta = {0.0f, 0.0f};

    // Keyboard state tables
    static inline std::array<bool, GLFW_KEY_LAST + 1> keysRealtime{};
    static inline std::array<bool, GLFW_KEY_LAST + 1> keysCurrent{};
    static inline std::array<bool, GLFW_KEY_LAST + 1> keysPrevious{};

    // Mouse state tables
    static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>
        buttonsRealtime{};
    static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> buttonsCurrent{};
    static inline std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>
        buttonsPrevious{};
};

} // namespace Engine
