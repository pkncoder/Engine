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
    : width(width), height(height), title(title) {

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
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    // Check to make sure the window actually got made
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    // Set the window context to our current window
    glfwMakeContextCurrent(window);

    // Initialize & check GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
    }

    // Set other settings
    setSettings();

    // Set the size change callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Set the intial window size
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    glViewport(0, 0, bufferWidth, bufferHeight);
}

// Deconstructor - Kill glfw
Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Wrapper to clean up constructor, sets the settings
void Window::setSettings() {

    // Turn off VSCNC
    glfwSwapInterval(0);

    // Turn on the OpenGL depth test
    glEnable(GL_DEPTH_TEST);
}

// Checking for closing the window
bool Window::shouldClose() const { return glfwWindowShouldClose(window); }

// Polling & swapping buffers
void Window::pollEvents() { glfwPollEvents(); }
void Window::swapBuffers() { glfwSwapBuffers(window); }

// Pre-frame window steps
void Window::preFrame() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // TODO: Default value somewhere else?
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Window update
void Window::postFrame() {
    this->swapBuffers();
    this->pollEvents();
}

// Get the framebuffer size
void Window::getSize(int &width, int &height) const {
    // We use FramebufferSize because on Retina displays,
    // WindowSize != PixelSize. This fixes your "cut off" bug.
    glfwGetFramebufferSize(window, &width, &height);
}

// Get the framebuffer aspect ratio
float Window::getAspectRatio() {

    int width, height;
    this->getSize(width,
                  height); // Now width and height will actually be updated

    if (height == 0)
        return 1.0f; // Safety check to avoid dividing by zero

    return (float)width / (float)height;
}

} // namespace Engine
