#pragma once

#include "Logger.h"

#include <functional>
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
    static float getFPS() { return currentFPS; }
    static float getAverageFPS() { return averageFPS; }

    // Profiling logic
    static void beginProfile(const std::string &name);
    static void endProfile(const std::string &name,
                           const LogType logType = LogType::IN_PLACE);
    static void periodicRun(int period, std::function<void()> function);

    // Output relevant data to console
    static void logPerformance(bool clearTerminal = true);

  private:
    // Delta time and delta time math
    static inline float deltaTime = 0.0;
    static inline double lastFrameTime = 0.0;

    // Total time
    static inline double totalTime = 0.0;

    // Current & average fps
    static inline float currentFPS = 0.0;
    static inline float averageFPS = 0.0;

    // Periodic printing logic lock
    static inline bool periodPrintLock = false;

    // Profile results
    static inline std::map<std::string, double> profileResults;
    static inline std::unordered_map<std::string, double> activeProfiles;
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
#define END_PROFILE_STACKED_LOG(name)                                          \
    Engine::Timer::endProfile(name, LogType::STACKED)

} // namespace Engine
