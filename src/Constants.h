#pragma once

#include <glm/glm.hpp>

namespace Engine {
namespace Constants {

namespace Asset {

inline constexpr const char *DEFAULT_MATERIAL_NAME = "ENG_Default";
inline constexpr const char *MATERIAL_ROOT_RELATIVE_PATH = "assets/materials/";
inline constexpr const char *MATERIAL_ID_PREFIX = "##Mn_";

} // namespace Asset

namespace Entity {

inline const uint32_t MAX_ENTITIES = 10000;
inline const uint8_t MAX_COMPONENTS = 32;

} // namespace Entity

// Logger Constants
namespace Logger {

inline const int MAX_STACKED_PENDING = 100;
inline const int MAX_IN_PLACE_PENDING = 30;

} // namespace Logger

namespace PathTracer {

inline const size_t MAX_INSTANCES = 10000;

} // namespace PathTracer

} // namespace Constants
} // namespace Engine
