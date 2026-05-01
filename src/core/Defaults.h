#pragma once

#include <glm/glm.hpp>

// TODO: CRAPPY FORMATTED FILE
namespace Engine {
namespace Defaults {

// Default Camera Settings
namespace Camera {
const glm::vec3 START_POSITION = glm::vec3(0.0f, 0.0f, 3.0f);
} // namespace Camera

// Window Settings
namespace Window {

// Widow height
const int START_WIDTH = 400;
const int START_HEIGHT = 400;

// Start title
inline constexpr const char *START_TITLE = "Engine";

// Clear color values
const float CLEAR_RED = 0.1f;
const float CLEAR_GREEN = 0.1f;
const float CLEAR_BLUE = 0.1f;
const float CLEAR_ALPHA = 1.0f;
} // namespace Window

} // namespace Defaults
} // namespace Engine
