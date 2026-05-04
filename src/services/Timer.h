#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>

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

    // Getters
    static float getDeltaTime() { return deltaTime; }
    static double getTotalTime() { return totalTime; }
    static uint32_t getTotalFrames() { return totalFrames; }
    static float getFPS() { return (int)(1.0f / deltaTime); }
    static float getAverageFPS() { return averageFPS; }

    // Profiling logic
    static void beginProfile(const std::string &name);
    static void endProfile(const std::string &name);

    // Output relevant data to console
    static void logPerformance(bool clearTerminal = true);

  private:
    // Delta time and delta time math
    static float deltaTime;
    static double lastFrameTime;

    // Total time + frames count
    static double totalTime;
    static uint32_t totalFrames;

    // FPS/Logging state
    static float averageFPS;
    static float ms;
    static float lastLogTime;

    // Profile results
    static std::map<std::string, double> profileResults;
    static std::unordered_map<std::string, double> activeProfiles;
};

// Timer that can either start and stop on const & scope, or manually keyed
struct ScopedProfiler {
    // Key / id for the timer
    std::string name;

    ScopedProfiler(const std::string &name) : name(name) {
        Timer::beginProfile(name);
    } // Constructor - Starts timer
    ~ScopedProfiler() { Timer::endProfile(name); } // Deconstructor - Ends timer
};

// Macro to create a scoped profiler
#define SCOPED_PROFILE(name) Engine::ScopedProfiler timer##__LINE__(name)

// Macro to start and stop profilers
#define START_PROFILE(name) Engine::Timer::beginProfile(name)
#define END_PROFILE(name) Engine::Timer::endProfile(name)

} // namespace Engine
