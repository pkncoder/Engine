#include "Window.h"

#include "../services/Logger.h"
#include "states/WindowState.h"

namespace Engine {

Window::Window(EngineState &state) {

    engineState = &state;

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
    window = glfwCreateWindow(
        engineState->window.width, engineState->window.height,
        engineState->window.settings.title.c_str(), nullptr, nullptr);

    // Check to make sure the window actually got made
    if (!window) {
        Logger::fatal("SYSTEM", "Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    // Set the window context to our current window
    glfwMakeContextCurrent(window);

    // Initialize & check GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::fatal("SYSTEM", "Failed to initialize GLAD");
    }

    // Set other settings
    setSettings();
    // Set the intial window size
    glfwGetFramebufferSize(window, &engineState->window.width,
                           &engineState->window.height);
    glViewport(0, 0, engineState->window.width, engineState->window.height);

    glClearColor(engineState->window.settings.clear_red,
                 engineState->window.settings.clear_green,
                 engineState->window.settings.clear_blue,
                 engineState->window.settings.clear_alpha);
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
