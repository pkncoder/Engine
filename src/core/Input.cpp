#include "Input.h"

// Initialize static members w/ dud values
GLFWwindow *Input::s_Window = nullptr;
glm::vec2 Input::s_LastMousePos = {0.0f, 0.0f};
glm::vec2 Input::s_MouseDelta = {0.0f, 0.0f};

// Input "service" init
void Input::Init(GLFWwindow *window) {

    // Save the window pointer
    s_Window = window;

    // Initialize mouse position so the first delta isn't a massive jump
    double x, y;
    glfwGetCursorPos(s_Window, &x, &y);
    s_LastMousePos = {(float)x, (float)y};
}

// Update polling
// TODO: Name Change?
void Input::Update() {

    // Get current position
    glm::vec2 currentPos = GetMousePosition();

    // Calculate how much the mouse moved since the last frame & save
    s_MouseDelta = currentPos - s_LastMousePos;
    s_LastMousePos = currentPos;
}

bool Input::IsKeyPressed(int keycode) {
    int state = glfwGetKey(s_Window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(int button) {
    int state = glfwGetMouseButton(s_Window, button);
    return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition() {

    double x, y;
    glfwGetCursorPos(s_Window, &x, &y);

    return {(float)x, (float)y};
}

glm::vec2 Input::GetMouseDelta() { return s_MouseDelta; }
