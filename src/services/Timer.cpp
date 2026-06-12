#include "Timer.h"

#include <GLFW/glfw3.h>

#include <string>

namespace Engine {

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

    // Get the instantanius fps and update the average
    currentFPS = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
    averageFPS = (currentFPS * 0.05f) + (averageFPS * (1.0f - 0.05f));
}

// Start profile
void Timer::beginProfile(const std::string &name) {
    activeProfiles[name] = glfwGetTime();
}

// Stop profile
void Timer::endProfile(const std::string &name, const LogType logType) {
    auto it = activeProfiles.find(name);
    if (it != activeProfiles.end()) {
        double duration = (glfwGetTime() - it->second) * 1000.0; // to ms
        profileResults[name] = duration;
        activeProfiles.erase(it);
    }

    Logger::info("PROFILE",
                 name + ": " +
                     std::to_string(profileResults[name]).substr(0, 6) + "ms",
                 logType);
}

// Example: Timer::periodicRun(2, []() { Logger::inf("EXAMPLE", "All of the
// bagels please."); });
// TODO: Fix - DOES NOT WORK WITH 1 SECOND OR FRACTIONAL INTERVALS
void Timer::periodicRun(int period, std::function<void()> function) {
    if ((int)totalTime % period == 0) {
        if (!periodPrintLock) {
            function();

            periodPrintLock = true;
        }
    } else {
        periodPrintLock = false;
    }
}

} // namespace Engine
