#include "UUIDGenerator.h"

#include <random>

namespace Engine {

UUID UUIDGenerator::generate() {
    std::random_device randomDevice;
    std::mt19937_64 engine(randomDevice());
    std::uniform_int_distribution<uint64_t> uniformDistribution;

    return uniformDistribution(engine);
}

} // namespace Engine
