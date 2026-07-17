#pragma once

#include <glm/glm.hpp>

namespace Engine {
namespace Defaults {

// Default Camera Settings
namespace Camera {

// Positioning
inline const glm::vec3 POSITION = glm::vec3(-2.0f, 0.0f, 3.0f);

// Camera speeds
inline const float SENSITIVITY = 0.25f;
inline const float MOVEMENT_SPEED = 20.0f;

// Camera angles
inline const float YAW = 0.0f;
inline const float PITCH = 0.0f;
inline const float FOV = 45.0f;

} // namespace Camera

// Window Settings
namespace Window {

// Widow size
inline const int WIDTH = 700;
inline const int HEIGHT = 700;

// Start title
inline constexpr const char *TITLE = "Engine";

// Clear color values
inline const float CLEAR_RED = 0.1f;
inline const float CLEAR_GREEN = 0.1f;
inline const float CLEAR_BLUE = 0.1f;
inline const float CLEAR_ALPHA = 1.0f;

} // namespace Window

} // namespace Defaults
} // namespace Engine
