#pragma once

#include <cstdint>

namespace Engine {

class UUID {

  public:
    UUID();

    operator uint64_t() const { return uuid; }

  private:
    uint64_t uuid = 0;
};
} // namespace Engine
