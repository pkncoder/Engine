#pragma once

#include "IEvent.h"

#include <glm/glm.hpp>

namespace Engine {

// Window resize event
struct WindowResizeEvent : public IEvent {
  public:
    // Takes in the new width & height
    inline WindowResizeEvent(const int newWidth, const int newHeight)
        : windowSize(glm::ivec2{newWidth, newHeight}) {}

    // Event type return
    inline EventType getType() const override {
        return EventType::WINDOW_RESIZE_EVENT;
    }

  public:
    // iVec2 (glm) for window size
    glm::ivec2 windowSize;
};
} // namespace Engine
