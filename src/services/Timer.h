#pragma once

#include "Logger.h"

#include <functional>
#include <map>
#include <string>
#include <unordered_map>

namespace Engine {

// Profile state informatino
struct ProfileResult {
  public:
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
    static inline double getTotalTime() { return totalTime; }
    static inline float getDeltaTime() { return deltaTime; }
    static inline float getFPS() { return currentFPS; }
    static inline float getAverageFPS() { return averageFPS; }

    // Profiling logic
    static void beginProfile(const std::string &name);
    static void endProfile(const std::string &name,
                           const LogType logType = LogType::IN_PLACE);

    // Periodic run logic
    // TODO: Create a macro
    static void beginPeriodicTask(const std::string &id, const float period,
                                  const std::function<void()> function);
    static void endPeriodicTask(const std::string &id);

  private:
    // Delta time& delta state variable
    static inline float deltaTime = 0.0;
    static inline double lastFrameTime = 0.0;

    // Total time
    static inline double totalTime = 0.0;

    // Current & average fps
    static inline float currentFPS = 0.0;
    static inline float averageFPS = 0.0;

    // Periodic printing logic lock
    static inline bool periodPrintLock = false;

    // Periodic run map for tracking
    static inline std::unordered_map<std::string, double>
        periodicTimerRegistry{};

    // Profile state maps
    static inline std::unordered_map<std::string, double> activeProfiles;
    static inline std::map<std::string, double> profileResults;
};

// Timer that can either start and stop on const & scope, or manually keyed
struct ScopedProfiler {
  public:
    ScopedProfiler(const std::string &name) : name(name) {
        Timer::beginProfile(name);
    }                                              // Constructor - Starts timer
    ~ScopedProfiler() { Timer::endProfile(name); } // Deconstructor - Ends timer

  public:
    // Key / id for the timer
    std::string name;
};

// Macro to create a scoped profiler
#define SCOPED_PROFILE(name) Engine::ScopedProfiler timer##__LINE__(name)

// Macro to start and stop profilers
#define START_PROFILE(name) Engine::Timer::beginProfile(name)
#define END_PROFILE(name) Engine::Timer::endProfile(name)
#define END_PROFILE_STACKED_LOG(name)                                          \
    Engine::Timer::endProfile(name, LogType::STACKED)

} // namespace Engine
