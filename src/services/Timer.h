#pragma once

namespace Engine {

class Timer {
  public:
    // Initializes the timer (call in Application::init)
    static void init();

    // Updates delta time and FPS logic (call at start of Application loop)
    static void update();

    // Getters for application logic
    static float getDeltaTime() { return deltaTime; }
    static float getFPS() { return fps; }
    static float getMS() { return ms; }

  private:
    static float deltaTime;
    static float lastFrameTime;

    // FPS Tracking
    static float fps;
    static float ms;
    static float lastFPSUpdateTime;
    static int frameCount;
};

} // namespace Engine
