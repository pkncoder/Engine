#include "Timer.h"

#include <GLFW/glfw3.h>

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

    // Update total
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

// Example: Timer::periodicRun("Bagel Reminder", 2.0f, []() {
// Logger::info("EXAMPLE", "All of the bagels please."); });
void Timer::beginPeriodicTask(const std::string &id, const float period,
                              const std::function<void()> function) {
    if (period <= 0.0f) // Avoid zero-periods
        return;

    // Look for this id, and if it doesn't exist: create it
    if (periodicTimerRegistry.find(id) == periodicTimerRegistry.end()) {
        periodicTimerRegistry[id] = totalTime;
    }

    // Get a reference to the tracking veriable
    double &lastCallbackTime = periodicTimerRegistry[id];

    // Check if enough time is passed
    if (totalTime - lastCallbackTime >= period) {

        // Do the function
        function();

        // Add the time to the period
        lastCallbackTime += period;

        // If 2 periods have already passed (ex. halt on model load), skip them
        if (totalTime - lastCallbackTime > period) {
            lastCallbackTime = totalTime;
        }
    }
}

// Stop a period task
void Timer::endPeriodicTask(const std::string &id) {
    periodicTimerRegistry.erase(id);
}

} // namespace Engine
