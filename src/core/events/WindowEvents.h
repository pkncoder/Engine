#pragma once

#include "IEvent.h"

#include <glm/glm.hpp>

namespace Engine {

struct WindowResizeEvent : public IEvent {
  public:
    glm::ivec2 windowSize;

    inline WindowResizeEvent(int newWidth, int newHeight)
        : windowSize(glm::ivec2{newWidth, newHeight}) {}

    inline EventType getType() const override {
        return EventType::WINDOW_RESIZE_EVENT;
    }
};
} // namespace Engine
