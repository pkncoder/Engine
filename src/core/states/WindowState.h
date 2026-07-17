#pragma once

#include "Defaults.h"
#include "WindowSettings.h"

namespace Engine {

struct WindowState {
  public:
    // Window configurations
    WindowSettings settings;

    // Window size
    int width = Defaults::Window::WIDTH;
    int height = Defaults::Window::HEIGHT;

    // Window aspect ratio
    float aspectRatio =
        Defaults::Window::WIDTH / (float)Defaults::Window::HEIGHT;
};

} // namespace Engine
