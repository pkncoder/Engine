#pragma once

#include <glm/glm.hpp>

// TODO: Expand upon this and actually use it
namespace Engine {
namespace Defaults {

// Default Camera Settings
namespace Camera {
const glm::vec3 START_POSITION = glm::vec3(0.0f, 0.0f, 3.0f);
} // namespace Camera

// Window Settings
namespace Window {
const int START_WIDTH = 400;
const int START_HEIGHT = 400;
const char *START_TITLE = "Engine - Phase II";
} // namespace Window

} // namespace Defaults
} // namespace Engine
