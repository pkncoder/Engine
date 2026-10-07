#pragma once

#include "../enums/ResourceType.h"

namespace Engine {

struct IGPUResource {
  public:
    void bind();
    void unbind();

  public:
    ResourceType resourceType;
};

} // namespace Engine
