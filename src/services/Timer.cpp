#include "Timer.h"

#include <GLFW/glfw3.h>

#include <iomanip>
#include <iostream>

namespace Engine {

// Starting values
// Delta time
float Timer::deltaTime = 0.0f;
double Timer::lastFrameTime = 0.0f;

// Total time + frames
double Timer::totalTime = 0.0f;
uint32_t Timer::totalFrames = 0;

// FPS
float Timer::fps = 0.0f;
float Timer::ms = 0.0f;
float Timer::lastLogTime = 0.0f;

// Profiling
std::map<std::string, double> Timer::profileResults;
std::unordered_map<std::string, double> Timer::activeProfiles;

// Initalize the starting log values
void Timer::init() { lastFrameTime = glfwGetTime(); }

// Update the tracked times
void Timer::update() {

    // Update delta time
    double currentTime = glfwGetTime();
    deltaTime = (float)(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    // Update totals
    totalTime = currentTime;
    totalFrames++;

    // Calculate rolling average stats
    if (currentTime - lastLogTime >= 1.0) {
        fps = (float)totalFrames / (float)totalTime; // Simple average
        ms = deltaTime * 1000.0f;
    }
}

// Start profile
void Timer::beginProfile(const std::string &name) {
    activeProfiles[name] = glfwGetTime();
}

// Stop profile
void Timer::endProfile(const std::string &name) {
    auto it = activeProfiles.find(name);
    if (it != activeProfiles.end()) {
        double duration = (glfwGetTime() - it->second) * 1000.0; // to ms
        profileResults[name] = duration;
        activeProfiles.erase(it);
    }
}

// Log time data
void Timer::logPerformance(bool clearTerminal) {

    // Clear terminal (ansi)
    if (clearTerminal)
        std::cout << "\x1B[2J\x1B[H";

    // Log performance
    std::cout << "\n--- ENGINE PERFORMANCE LOG ---" << std::endl;
    std::cout << "FPS: " << (int)(1.0f / deltaTime) << " | Avg: " << (int)fps
              << std::endl;
    std::cout << "Total Frames: " << totalFrames
              << " | Total Time: " << (int)totalTime << "s" << std::endl;

    // Log scoped profiles
    for (auto const &[name, duration] : profileResults) {
        std::cout << "  " << name << ": " << std::fixed << std::setprecision(4)
                  << duration << "ms" << std::endl;
    }

    lastLogTime = (float)glfwGetTime();
}

} // namespace Engine
