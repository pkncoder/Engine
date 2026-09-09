#pragma once

#include <cstdint>

namespace Engine {

using UUID = uint64_t;

class UUIDGenerator {

  public:
    static UUID generate();
};

} // namespace Engine
