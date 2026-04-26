#include "Input.h"
#include <GLFW/glfw3.h>

// Initialize static members
GLFWwindow *Input::s_Window = nullptr;
glm::vec2 Input::s_LastMousePos = {0.0f, 0.0f};
glm::vec2 Input::s_MouseDelta = {0.0f, 0.0f};

void Input::Init(GLFWwindow *window) {
    s_Window = window;

    // Initialize mouse position so the first delta isn't a massive jump
    double x, y;
    glfwGetCursorPos(s_Window, &x, &y);
    s_LastMousePos = {(float)x, (float)y};
}

void Input::Update() {
    glm::vec2 currentPos = GetMousePosition();

    // Calculate how much the mouse moved since the last frame
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
