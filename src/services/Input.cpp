#include "Input.h"
#include "Logger.h"
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

    // Register the callbacks to our input methods
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

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

void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
    // Check key button bounds
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            keysRealtime[key] = true;
        if (action == GLFW_RELEASE)
            keysRealtime[key] = false;
    }
}

void Input::mouseButtonCallback(GLFWwindow *window, int button, int action,
                                int mods) {
    // Check mouse button bounds
    if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
        if (action == GLFW_PRESS)
            buttonsRealtime[button] = true;
        if (action == GLFW_RELEASE)
            buttonsRealtime[button] = false;
    }
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

} // namespace Engine
