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

void Timer::init() {
    lastFrameTime = (float)glfwGetTime();
    lastFPSUpdateTime = lastFrameTime;
}

void Timer::update() {
    float currentTime = (float)glfwGetTime();

    // 1. Calculate Delta Time
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // 2. Calculate FPS and MS every 1 second
    frameCount++;
    if (currentTime - lastFPSUpdateTime >= 1.0f) {
        fps = (float)frameCount;
        ms = 1000.0f / fps;

        // Output to console (you can disable this later when you have a UI)
        std::cout << fps << " FPS (" << ms << " ms/frame)" << std::endl;

        frameCount = 0;
        lastFPSUpdateTime += 1.0f;
    }
}

} // namespace Engine
