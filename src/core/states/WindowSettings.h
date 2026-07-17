#pragma once

#include "Defaults.h"

#include <string>

namespace Engine {

struct WindowSettings {
  public:
    // Window title
    std::string title = Defaults::Window::TITLE;

    // Clear color values
    float clearRed = Defaults::Window::CLEAR_RED;
    float clearGreen = Defaults::Window::CLEAR_GREEN;
    float clearBlue = Defaults::Window::CLEAR_BLUE;
    float clearAlpha = Defaults::Window::CLEAR_ALPHA;
};

} // namespace Engine
