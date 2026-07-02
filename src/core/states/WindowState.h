#pragma once

#include "Defaults.h"

#include <string>

namespace Engine {

struct WindowState {
  public:
    // Window title
    std::string title = Defaults::Window::TITLE;

    // Window size
    unsigned int width = Defaults::Window::WIDTH;
    unsigned int height = Defaults::Window::HEIGHT;

    // Clear color values
    float clear_red = Defaults::Window::CLEAR_RED;
    float clear_green = Defaults::Window::CLEAR_GREEN;
    float clear_blue = Defaults::Window::CLEAR_BLUE;
    float clear_alpha = Defaults::Window::CLEAR_ALPHA;
};

} // namespace Engine
