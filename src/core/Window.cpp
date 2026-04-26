#include "glad/glad.h"

#include "Window.h"
#include <iostream>

namespace Engine {

// Code ran when window size is changed
// TODO: Here?
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Change the viewport size
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, const std::string &title)
    : m_Width(width), m_Height(height), m_Title(title) {

    // Initialize & check for error for glfw
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Set version information of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the GLFW window and give it to our wrapper
    m_Window =
        glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

    // Check to make sure the window actually got made
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    // Set the window context to our current window
    glfwMakeContextCurrent(m_Window);

    // Initialize & check GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    // TODO: Where should I put settings

    // Turn on the OpenGL depth test
    glEnable(GL_DEPTH_TEST);

    // Turn off VSCNC
    glfwSwapInterval(0); // TODO: Decide to keep or not

    // Set the size change callback
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    // Set the intial window size
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(m_Window, &bufferWidth, &bufferHeight);
    glViewport(0, 0, bufferWidth, bufferHeight);
}

// Deconstructor - Kill glfw
Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

// Checking for closing the window
bool Window::ShouldClose() const { return glfwWindowShouldClose(m_Window); }

// Polling & swapping buffers
void Window::PollEvents() { glfwPollEvents(); }
void Window::SwapBuffers() { glfwSwapBuffers(m_Window); }

// Window update
void Window::OnUpdate() {
    this->SwapBuffers();
    this->PollEvents();
}

// Get the framebuffer size
void Window::GetSize(int &width, int &height) const {
    // We use FramebufferSize because on Retina displays,
    // WindowSize != PixelSize. This fixes your "cut off" bug.
    glfwGetFramebufferSize(m_Window, &width, &height);
}

// Get the framebuffer aspect ratio
float Window::GetAspectRatio() {

    int width, height;
    this->GetSize(width,
                  height); // Now width and height will actually be updated

    if (height == 0)
        return 1.0f; // Safety check to avoid dividing by zero

    return (float)width / (float)height;
}

} // namespace Engine
