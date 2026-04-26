#include "Timer.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Engine {

// Initialize static variables
float Timer::deltaTime = 0.0f;
float Timer::lastFrameTime = 0.0f;
float Timer::fps = 0.0f;
float Timer::ms = 0.0f;
float Timer::lastFPSUpdateTime = 0.0f;
int Timer::frameCount = 0;

// Initialize values
void Timer::init() {
    lastFrameTime = (float)glfwGetTime();
    lastFPSUpdateTime = lastFrameTime;
}

// Update phase, akin to input polling
void Timer::update() {

    // Get the current time
    float currentTime = (float)glfwGetTime();

    // Calculate deltaTime and update the lastFrameTime
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // Increase the frame count
    frameCount++;

    // TODO: Move this into a logging thing
    if (currentTime - lastFPSUpdateTime >= 1.0f) {
        fps = (float)frameCount;
        ms = 1000.0f / fps;

        // Output to console
        // TODO: Temp
        std::cout << fps << " FPS (" << ms << " ms/frame)" << std::endl;

        // Reset frameCount and increase the lastFPS update time
        // TODO: Don't do it like that
        frameCount = 0;
        lastFPSUpdateTime += 1.0f;
    }
}

} // namespace Engine
