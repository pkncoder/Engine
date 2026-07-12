#pragma once

#include <glm/glm.hpp>

namespace Engine {
namespace Defaults {

// Default Camera Settings
namespace Camera {

// Camera angles
const float YAW = 0.0f;
const float PITCH = 0.0f;

// Camera movement
const glm::vec3 POSITION = glm::vec3(-2.0f, 0.0f, 3.0f);
const float MOVEMENT_SPEED = 20.0f;

// Camera looking movement
const float SENSITIVITY = 0.25f;
const float FOV = 45.0f;

} // namespace Camera

// Window Settings
namespace Window {

// Widow size
const int WIDTH = 700;
const int HEIGHT = 700;

// Start title
inline constexpr const char *TITLE = "Engine";

// Clear color values
const float CLEAR_RED = 0.1f;
const float CLEAR_GREEN = 0.1f;
const float CLEAR_BLUE = 0.1f;
const float CLEAR_ALPHA = 1.0f;

} // namespace Window

} // namespace Defaults
} // namespace Engine
