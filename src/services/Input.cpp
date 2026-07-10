#include "Input.h"
#include "../core/events/KeyEvents.h"
#include "../core/events/MouseEvents.h"
#include "../core/events/WindowEvents.h"
#include "Logger.h"
#include <GLFW/glfw3.h>
#include <cstring> // for std::memcpy

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
    glfwSetFramebufferSizeCallback(window, framebufferSizeEventCallback);

    // Get the initial mouse pos
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    lastMousePos = {(float)x, (float)y};

    Logger::info("SYSTEM", "Input service initialized.");
}

void Input::update() {
    // Roll back the cycle history arrays
    keysPrevious = keysCurrent;
    buttonsPrevious = buttonsCurrent;

    // Snapshot the real-time callback arrays into the stable cycle arrays
    keysCurrent = keysRealtime;
    buttonsCurrent = buttonsRealtime;

    // 3. Process mouse tracking metrics
    glm::vec2 currentPos = getMousePosition();
    mouseDelta = currentPos - lastMousePos;
    lastMousePos = currentPos;
}

// Return true if a key is currently pressed
bool Input::isKeyPressed(int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return keysCurrent[keycode];
}

// Return true if a key has been pressed this cycle
bool Input::isKeyJustPressed(int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return keysCurrent[keycode] && !keysPrevious[keycode];
}

// Return true if a key has been pressed last cycle & is pressed
bool Input::isKeyHeld(int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return keysCurrent[keycode] && keysPrevious[keycode];
}

// Return true if a key stopped being pressed this cycle
bool Input::isKeyLetGo(int keycode) {
    if (keycode < 0 || keycode > GLFW_KEY_LAST)
        return false;
    return !keysCurrent[keycode] && keysPrevious[keycode];
}

// Return true if a mouse button is currently pressed
bool Input::isMouseButtonPressed(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return buttonsCurrent[button];
}

// Return true if a mouse button has been pressed this cycle
bool Input::isMouseButtonJustPressed(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return buttonsCurrent[button] && !buttonsPrevious[button];
}

// Return true if a mouse button has been pressend & is pressed this cycle
bool Input::isButtonHeld(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return buttonsCurrent[button] && buttonsPrevious[button];
}

// Return true if a button has been let go this cycle
bool Input::isMouseButtonLetGo(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return !buttonsCurrent[button] && buttonsPrevious[button];
}

// Get current mouse pos
glm::vec2 Input::getMousePosition() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return {(float)x, (float)y};
}

// Code ran when window size is changed
void Input::keyEventCallback(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
    KeyCode keyCode = static_cast<KeyCode>(key);
    bool ctrl = (mods & GLFW_MOD_CONTROL) != 0;
    bool shift = (mods & GLFW_MOD_SHIFT) != 0;
    bool alt = (mods & GLFW_MOD_ALT) != 0;
    bool super = (mods & GLFW_MOD_SUPER) != 0;

    if (key > 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            keysRealtime[key] = true;
        } else if (action == GLFW_RELEASE) {
            keysRealtime[key] = false;
        }
    }

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
void Input::mouseButtonEventCallback(GLFWwindow *window, int button, int action,
                                     int mods) {
    MouseCode buttonCode = static_cast<MouseCode>(button);

    if (button > 0 && button <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            buttonsRealtime[button] = true;
        } else if (action == GLFW_RELEASE) {
            buttonsRealtime[button] = false;
        }
    }

    switch (action) {
    case GLFW_PRESS:
        dispatchEvent(std::make_shared<MouseButtonPressEvent>(buttonCode));
        return;
    case GLFW_RELEASE:
        dispatchEvent(std::make_shared<MouseButtonReleaseEvent>(buttonCode));
        return;
    }
}

// Code ran when window size is changed
void Input::cursorEventCallback(GLFWwindow *window, double xPos, double yPos) {
    dispatchEvent(std::make_shared<MouseMoveEvent>(xPos, yPos));
}

// Code ran when window size is changed
void Input::scrollEventCallback(GLFWwindow *window, double xOffset,
                                double yOffset) {
    dispatchEvent(std::make_shared<MouseScrollEvent>(xOffset, yOffset));
}

// Code ran when window size is changed
void Input::framebufferSizeEventCallback(GLFWwindow *window, int width,
                                         int height) {
    // Change the viewport size
    glViewport(0, 0, width, height);

    if (engineState) {
        engineState->window.width = width;
        engineState->window.height = height;

        engineState->window.aspectRatio = width / (float)height;
    }

    dispatchEvent(std::make_shared<WindowResizeEvent>(width, height));
}

} // namespace Engine
