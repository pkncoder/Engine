#include "Window.h"

#include "../services/Logger.h"
#include "states/WindowState.h"

namespace Engine {

// Code ran when window size is changed
void Window::framebufferSizeCallback(GLFWwindow *window, int width,
                                     int height) {
    // Change the viewport size
    glViewport(0, 0, width, height);

    Window *windowWrapperInstance =
        static_cast<Window *>(glfwGetWindowUserPointer(window));

    if (windowWrapperInstance && windowWrapperInstance->windowState) {
        windowWrapperInstance->windowState->width = width;
        windowWrapperInstance->windowState->height = height;

        windowWrapperInstance->windowState->aspectRatio = width / (float)height;
    }
}

Window::Window(WindowState &state) {

    windowState = &state;

    // Initialize & check for error for glfw
    if (!glfwInit()) {
        Logger::fatal("SYSTEM", "Failed to initialize GLFW");
        return;
    }

#ifdef __APPLE__
    // macOS is capped at 4.1 and requires Forward Compatibility
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    // Linux/Windows can use 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    // Create the GLFW window and give it to our wrapper
    window =
        glfwCreateWindow(windowState->width, windowState->height,
                         windowState->settings.title.c_str(), nullptr, nullptr);

    // Check to make sure the window actually got made
    if (!window) {
        Logger::fatal("SYSTEM", "Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    // Set the window context to our current window
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);

    // Initialize & check GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::fatal("SYSTEM", "Failed to initialize GLAD");
    }

    // Set other settings
    setSettings();

    // Set the size change callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Set the intial window size
    glfwGetFramebufferSize(window, &windowState->width, &windowState->height);
    glViewport(0, 0, windowState->width, windowState->height);

    glClearColor(
        windowState->settings.clear_red, windowState->settings.clear_green,
        windowState->settings.clear_blue, windowState->settings.clear_alpha);
}

// Deconstructor - Kill glfw
Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

// Wrapper to clean up constructor, sets the settings
void Window::setSettings() {

    // Turn off VSCNC
    glfwSwapInterval(0);

    // Turn on the OpenGL depth test
    glEnable(GL_DEPTH_TEST);

    // Turn on back face culling
    glEnable(GL_CULL_FACE);

    // Hardware anti-aliasing
    glEnable(GL_MULTISAMPLE);
}

// Checking for closing the window
bool Window::shouldClose() { return glfwWindowShouldClose(window); }

// Polling & swapping buffers
void Window::pollEvents() { glfwPollEvents(); }
void Window::swapBuffers() { glfwSwapBuffers(window); }

// Pre-frame window steps
void Window::preFrame() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

// Window update
void Window::postFrame() {
    swapBuffers();
    pollEvents();
}

} // namespace Engine
