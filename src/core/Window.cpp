#include "glad/glad.h"

#include "Window.h"
#include <iostream>

namespace Engine {

// 1. The Callback function
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Tell OpenGL the new size of the rendering area
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, const std::string &title)
    : m_Width(width), m_Height(height), m_Title(title) {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window =
        glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    // 2. Inside your Window Initialization (after glfwCreateWindow)
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    // 3. Crucial for Mac: Get the ACTUAL pixel dimensions for the initial
    // viewport
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(m_Window, &bufferWidth, &bufferHeight);
    glViewport(0, 0, bufferWidth, bufferHeight);

    glEnable(GL_DEPTH_TEST);
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::OnUpdate() {
    this->SwapBuffers();
    this->PollEvents();
}

void Window::GetSize(int &width, int &height) const {
    // We use FramebufferSize because on Retina displays,
    // WindowSize != PixelSize. This fixes your "cut off" bug.
    glfwGetFramebufferSize(m_Window, &width, &height);
}

float Window::GetAspectRatio() {
    int width, height;
    this->GetSize(width,
                  height); // Now width and height will actually be updated

    if (height == 0)
        return 1.0f; // Safety check to avoid dividing by zero
    return (float)width / (float)height;
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(m_Window); }
void Window::PollEvents() { glfwPollEvents(); }
void Window::SwapBuffers() { glfwSwapBuffers(m_Window); }
} // namespace Engine
