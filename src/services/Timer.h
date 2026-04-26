#pragma once

#include <map>
#include <string>

namespace Engine {

struct ProfileResult {
    std::string Name;
    double Time;
};

class Timer {
  public:
    // Initalizing the timer
    static void init();

    // Updating timer tracking data
    static void update();

    // Output relevant data to console
    static void log();

    // Getters
    static float getDeltaTime() { return deltaTime; }
    static double getTotalTime() { return totalTime; }
    static uint32_t getTotalFrames() { return totalFrames; }

    // Profiling logic
    static void stopProfiling(const std::string &name, double duration);
    static const std::map<std::string, double> &getProfileResults() {
        return profileResults;
    }

  private:
    // Delta time and delta time math
    static float deltaTime;
    static double lastFrameTime;

    // Total time + frames count
    static double totalTime;
    static uint32_t totalFrames;

    // FPS/Logging state
    static float fps;
    static float ms;
    static float lastLogTime;

    // Profile results
    static std::map<std::string, double> profileResults;
};

// Timer that is based on scope
struct ScopedTimer {

    // Names
    std::string name;
    double start;

    // Constructor & deconstructor
    ScopedTimer(const std::string &name); // Starts timer
    ~ScopedTimer();                       // Ends timer
};

// Timer that stops at the end of a scope
#define TIMER_SCOPE(name) Engine::ScopedTimer timer##__LINE__(name)

} // namespace Engine
