#include "Timer.h"
#include "Logger.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

namespace Engine {

// Starting values
// Delta time
float Timer::deltaTime = 0.0f;
double Timer::lastFrameTime = 0.0f;

// Total time + frames
double Timer::totalTime = 0.0f;
uint32_t Timer::totalFrames = 0;

// FPS
float Timer::averageFPS = 0.0f;
float Timer::ms = 0.0f;
float Timer::lastLogTime = 0.0f;

// Profiling
std::map<std::string, double> Timer::profileResults;
std::unordered_map<std::string, double> Timer::activeProfiles;

// Initalize the starting log values
void Timer::init() {
    lastFrameTime = glfwGetTime();

    Logger::info("SYSTEM", "Timer service initialized.");
}

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
        averageFPS = (float)totalFrames / (float)totalTime; // Simple average
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

    Logger::info("PROFILE",
                 name + ": " +
                     std::to_string(profileResults[name]).substr(0, 6) + "ms",
                 LogType::IN_PLACE);
}

} // namespace Engine
