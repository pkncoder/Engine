#include "Window.h"

#include "../services/Logger.h"
#include "events/IEventDispatcher.h"
#include "events/KeyEvents.h"
#include "events/MouseEvents.h"
#include "events/WindowEvents.h"
#include "states/WindowState.h"
#include <memory>

namespace Engine {

Window::Window(WindowState &state, IEventDispatcher &eventDispatcher) {

    windowState = &state;
    this->eventDispatcher = &eventDispatcher;

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
    glfwSetKeyCallback(window, keyEventCallback);
    glfwSetMouseButtonCallback(window, mouseButtonEventCallback);
    glfwSetCursorPosCallback(window, cursorEventCallback);
    glfwSetScrollCallback(window, scrollEventCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeEventCallback);

    // Initialize & check GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::fatal("SYSTEM", "Failed to initialize GLAD");
    }

    // Set other settings
    setSettings();
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

// Code ran when window size is changed
void Window::keyEventCallback(GLFWwindow *window, int key, int scancode,
                              int action, int mods) {
    Window *windowWrapperInstance =
        static_cast<Window *>(glfwGetWindowUserPointer(window));

    KeyCode keyCode = static_cast<KeyCode>(key);
    bool ctrl = (mods & GLFW_MOD_CONTROL) != 0;
    bool shift = (mods & GLFW_MOD_SHIFT) != 0;
    bool alt = (mods & GLFW_MOD_ALT) != 0;
    bool super = (mods & GLFW_MOD_SUPER) != 0;

    switch (action) {
    case GLFW_PRESS:
        windowWrapperInstance->eventDispatcher->dispatchEvent(
            std::make_shared<KeyPressEvent>(keyCode, ctrl, shift, alt, super));
        return;
    case GLFW_RELEASE:
        windowWrapperInstance->eventDispatcher->dispatchEvent(
            std::make_shared<KeyReleaseEvent>(keyCode, ctrl, shift, alt,
                                              super));
        return;
    case GLFW_REPEAT:
        windowWrapperInstance->eventDispatcher->dispatchEvent(
            std::make_shared<KeyRepeatEvent>(keyCode, ctrl, shift, alt, super));
        return;
    }
}

// Code ran when window size is changed
void Window::mouseButtonEventCallback(GLFWwindow *window, int button,
                                      int action, int mods) {
    Window *windowWrapperInstance =
        static_cast<Window *>(glfwGetWindowUserPointer(window));

    MouseCode buttonCode = static_cast<MouseCode>(button);

    switch (action) {
    case GLFW_PRESS:
        windowWrapperInstance->eventDispatcher->dispatchEvent(
            std::make_shared<MouseButtonPressEvent>(buttonCode));
        return;
    case GLFW_RELEASE:
        windowWrapperInstance->eventDispatcher->dispatchEvent(
            std::make_shared<MouseButtonReleaseEvent>(buttonCode));
        return;
    }
}

// Code ran when window size is changed
void Window::cursorEventCallback(GLFWwindow *window, double xPos, double yPos) {
    Window *windowWrapperInstance =
        static_cast<Window *>(glfwGetWindowUserPointer(window));

    windowWrapperInstance->eventDispatcher->dispatchEvent(
        std::make_shared<MouseMoveEvent>(xPos, yPos));
}

// Code ran when window size is changed
void Window::scrollEventCallback(GLFWwindow *window, double xOffset,
                                 double yOffset) {
    Window *windowWrapperInstance =
        static_cast<Window *>(glfwGetWindowUserPointer(window));

    windowWrapperInstance->eventDispatcher->dispatchEvent(
        std::make_shared<MouseScrollEvent>(xOffset, yOffset));
}

// Code ran when window size is changed
void Window::framebufferSizeEventCallback(GLFWwindow *window, int width,
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

    windowWrapperInstance->eventDispatcher->dispatchEvent(
        std::make_shared<WindowResizeEvent>(width, height));
}

} // namespace Engine
