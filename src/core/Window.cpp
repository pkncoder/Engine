#include "Window.h"

#include "../services/Logger.h"
#include "events/WindowEvents.h"
#include "states/EngineState.h"
#include "states/WindowState.h"
#include <GLFW/glfw3.h>

namespace Engine {

Window::Window(EngineState &state) {

    // Initialize & check for error for glfw
    if (!glfwInit()) {
        Logger::fatal("SYSTEM", "Failed to initialize GLFW");
        return;
    }

    // Opengl version setting
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
        glfwCreateWindow(state.window.width, state.window.height,
                         state.window.settings.title.c_str(), nullptr, nullptr);

    // Check to make sure the window actually got made
    if (!window) {
        Logger::fatal("SYSTEM", "Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    // Set the window context to our current window
    glfwMakeContextCurrent(window);

    // Create the window pointer for the static callback to access dispatchEvent
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeEventCallback);

    // Initialize & check GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::fatal("SYSTEM", "Failed to initialize GLAD");
    }

    // Set other settings window
    setSettings(state);

    // Set the intial window size
    glfwGetFramebufferSize(window, &state.window.width, &state.window.height);
    glViewport(0, 0, state.window.width, state.window.height);
}

// Deconstructor to clean up memory
Window::~Window() {

    // If window was make, delete it
    if (window) {
        glfwDestroyWindow(window);
    }

    // Kill glfw
    glfwTerminate();
}

// Wrapper to clean up constructor, sets the settings
void Window::setSettings(EngineState &state) const {

    // Turn off VSCNC
    // TODO: temp
    glfwSwapInterval(0);

    // Turn on the OpenGL depth test
    glEnable(GL_DEPTH_TEST);

    // Turn on back face culling
    glEnable(GL_CULL_FACE);

    // Hardware anti-aliasing
    glEnable(GL_MULTISAMPLE);

    // Clear color
    glClearColor(
        state.window.settings.clearRed, state.window.settings.clearGreen,
        state.window.settings.clearBlue, state.window.settings.clearAlpha);
}

// Pre-frame window steps
void Window::preFrame() const {

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Window update
void Window::postFrame() const {

    // Swap framebuffers and poll events
    swapBuffers();
    pollEvents();
}

// Check for the window being marked for death
bool Window::shouldClose() const { return glfwWindowShouldClose(window); }

// Polling & swapping buffers
void Window::pollEvents() const { glfwPollEvents(); }
void Window::swapBuffers() const { glfwSwapBuffers(window); }

// Code ran when window size is changed
void Window::framebufferSizeEventCallback(GLFWwindow *window, int width,
                                          int height) {
    // Change the viewport size
    glViewport(0, 0, width, height);

    // if (engineState) {
    //     engineState->window.width = width;
    //     engineState->window.height = height;
    //
    //     engineState->window.aspectRatio = width / (float)height;
    // }

    // Get the reference to the window & dispatch a resize event
    Window *windowReference =
        static_cast<Window *>(glfwGetWindowUserPointer(window));
    windowReference->dispatchEvent(
        std::make_shared<WindowResizeEvent>(width, height));
}

} // namespace Engine
