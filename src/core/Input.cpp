#include "Input.h"

// Initialize static members w/ dud values
GLFWwindow *Input::window = nullptr;
glm::vec2 Input::lastMousePos = {0.0f, 0.0f};
glm::vec2 Input::mouseDelta = {0.0f, 0.0f};

// Input "service" init
void Input::init(GLFWwindow *window_ptr) {

    // Save the window pointer
    window = window_ptr;

    // Initialize mouse position so the first delta isn't a massive jump
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    lastMousePos = {(float)x, (float)y};
}

// Update polling
// TODO: Name Change?
void Input::update() {

    // Get current position
    glm::vec2 currentPos = getMousePosition();

    // Calculate how much the mouse moved since the last frame & save
    mouseDelta = currentPos - lastMousePos;
    lastMousePos = currentPos;
}

bool Input::isKeyPressed(int keycode) {
    int state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonPressed(int button) {
    int state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

glm::vec2 Input::getMousePosition() {

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    return {(float)x, (float)y};
}

glm::vec2 Input::getMouseDelta() { return mouseDelta; }
