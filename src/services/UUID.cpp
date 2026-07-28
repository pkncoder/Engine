#include "UUID.h"

#include <random>

namespace Engine {

// Use static thread_local to ensure the generator is initialized once per
// thread
static std::random_device randomDevice;
static std::mt19937_64 engine(randomDevice());
static std::uniform_int_distribution<uint64_t> uniformDistribution;

UUID::UUID() { uuid = uniformDistribution(engine); }

} // namespace Engine
