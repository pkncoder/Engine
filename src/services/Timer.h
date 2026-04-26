#pragma once

namespace Engine {

class Timer {
  public:
    // Initializes the timer
    static void init();

    // Update timer values
    static void update();

    // Getters for application logic
    static float getDeltaTime() { return deltaTime; }
    static float getFPS() { return fps; }
    static float getMS() { return ms; }

  private:
    // Delta time and delta time logic
    static float deltaTime;
    static float lastFrameTime;

    // FPS Tracking
    static float fps;
    static float ms;
    static float lastFPSUpdateTime;
    static int frameCount;
};

} // namespace Engine
