#include "Input.h"

#include "../core/events/KeyEvents.h"
#include "../core/events/MouseEvents.h"
#include "Logger.h"

#include <GLFW/glfw3.h>

namespace Engine {

void Input::init(GLFWwindow *window_ptr) {

    // Save the window
    window = window_ptr;

    // Zero out out the state tables
    keysRealtime.fill(false);
    keysCurrent.fill(false);
    keysPrevious.fill(false);
    buttonsRealtime.fill(false);
    buttonsCurrent.fill(false);
    buttonsPrevious.fill(false);

    glfwSetKeyCallback(window, keyEventCallback);
    glfwSetMouseButtonCallback(window, mouseButtonEventCallback);
    glfwSetCursorPosCallback(window, cursorEventCallback);
    glfwSetScrollCallback(window, scrollEventCallback);

    // Get the initial mouse pos
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    lastMousePos = {(float)x, (float)y};

    Logger::info("SYSTEM", "Input service initialized.");
}

void Input::poll() {
    // Roll back the cycle history arrays
    keysPrevious = keysCurrent;
    buttonsPrevious = buttonsCurrent;

    // Snapshot the real-time callback arrays into the stable cycle arrays
    keysCurrent = keysRealtime;
    buttonsCurrent = buttonsRealtime;

    // Process mouse tracking metrics
    glm::vec2 currentPos = getMousePosition();
    mouseDelta = currentPos - lastMousePos;
    lastMousePos = currentPos;
}

// Return true if a key is currently pressed
bool Input::isKeyPressed(const int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return keysCurrent[keycode];
}

// Return true if a key has been pressed this cycle
bool Input::isKeyJustPressed(const int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return keysCurrent[keycode] && !keysPrevious[keycode];
}

// Return true if a key has been pressed last cycle & is pressed
bool Input::isKeyHeld(const int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return keysCurrent[keycode] && keysPrevious[keycode];
}

// Return true if a key stopped being pressed this cycle
bool Input::isKeyLetGo(const int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return !keysCurrent[keycode] && keysPrevious[keycode];
}

// Return true if a mouse button is currently pressed
bool Input::isMouseButtonPressed(const int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return buttonsCurrent[button];
}

// Return true if a mouse button has been pressed this cycle
bool Input::isMouseButtonJustPressed(const int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return buttonsCurrent[button] && !buttonsPrevious[button];
}

// Return true if a mouse button has been pressend & is pressed this cycle
bool Input::isButtonHeld(const int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return buttonsCurrent[button] && buttonsPrevious[button];
}

// Return true if a button has been let go this cycle
bool Input::isMouseButtonLetGo(const int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return !buttonsCurrent[button] && buttonsPrevious[button];
}

// Get current mouse pos
glm::vec2 Input::getMousePosition() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return {(float)x, (float)y}; // vec2
}

// Code ran when window size is changed
void Input::keyEventCallback(GLFWwindow *window, const int key,
                             const int scancode, const int action,
                             const int mods) {
    // Key states
    KeyCode keyCode = static_cast<KeyCode>(key);
    bool ctrl = (mods & GLFW_MOD_CONTROL) != 0;
    bool shift = (mods & GLFW_MOD_SHIFT) != 0;
    bool alt = (mods & GLFW_MOD_ALT) != 0;
    bool super = (mods & GLFW_MOD_SUPER) != 0;

    // CHeck for key bounds
    if (key <= 0 && key > GLFW_KEY_LAST) {
        return;
    }

    // Set the realtime key states
    if (action == GLFW_PRESS) {
        keysRealtime[key] = true;
    } else if (action == GLFW_RELEASE) {
        keysRealtime[key] = false;
    }

    // Dispatch events based on action
    switch (action) {
    case GLFW_PRESS:
        dispatchEvent(
            std::make_shared<KeyPressEvent>(keyCode, ctrl, shift, alt, super));
        return;
    case GLFW_RELEASE:
        dispatchEvent(std::make_shared<KeyReleaseEvent>(keyCode, ctrl, shift,
                                                        alt, super));
        return;
    case GLFW_REPEAT:
        dispatchEvent(
            std::make_shared<KeyRepeatEvent>(keyCode, ctrl, shift, alt, super));
        return;
    }
}

// Code ran when window size is changed
void Input::mouseButtonEventCallback(GLFWwindow *window, const int button,
                                     const int action, const int mods) {

    // Button state
    MouseCode buttonCode = static_cast<MouseCode>(button);

    if (button > 0 && button <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            buttonsRealtime[button] = true;
        } else if (action == GLFW_RELEASE) {
            buttonsRealtime[button] = false;
        }
    }

    // Dispatch events based on action
    switch (action) {
    case GLFW_PRESS:
        dispatchEvent(std::make_shared<MouseButtonPressEvent>(buttonCode));
        return;
    case GLFW_RELEASE:
        dispatchEvent(std::make_shared<MouseButtonReleaseEvent>(buttonCode));
        return;
    }
}

// Dispatch event for cursor state change
void Input::cursorEventCallback(GLFWwindow *window, const double xPos,
                                const double yPos) {
    dispatchEvent(std::make_shared<MouseMoveEvent>(xPos, yPos));
}

// Dispatch event for scroll state change
void Input::scrollEventCallback(GLFWwindow *window, const double xOffset,
                                const double yOffset) {
    dispatchEvent(std::make_shared<MouseScrollEvent>(xOffset, yOffset));
}

} // namespace Engine
