#pragma once

#include "IEvent.h"

#include <glm/glm.hpp>

namespace Engine {

struct WindowResizeEvent : public IEvent {
  public:
    inline WindowResizeEvent(const int newWidth, const int newHeight)
        : windowSize(glm::ivec2{newWidth, newHeight}) {}

    inline EventType getType() const override {
        return EventType::WINDOW_RESIZE_EVENT;
    }

  public:
    glm::ivec2 windowSize;
};
} // namespace Engine
