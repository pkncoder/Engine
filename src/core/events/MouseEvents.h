#pragma once

#include "IEvent.h"

#include <glm/fwd.hpp>

namespace Engine {

using MouseCode = glm::uint16_t;

namespace Mouse {

enum : MouseCode {
    // From glfw3.h
    MOUSE_BUTTON_1 = 0,
    MOUSE_BUTTON_2 = 1,
    MOUSE_BUTTON_3 = 2,
    MOUSE_BUTTON_4 = 3,
    MOUSE_BUTTON_5 = 4,
    MOUSE_BUTTON_6 = 5,
    MOUSE_BUTTON_7 = 6,
    MOUSE_BUTTON_8 = 7,
    MOUSE_LEFT = MOUSE_BUTTON_1,
    MOUSE_RIGHT = MOUSE_BUTTON_2,
    MOUSE_MIDDLE = MOUSE_BUTTON_3
};

}

struct MouseMoveEvent : public IEvent {
  public:
    inline MouseMoveEvent(const double xMousePos, const double yMousePos)
        : xPos(xMousePos), yPos(yMousePos) {}

    inline EventType getType() const override {
        return EventType::MOUSE_MOVE_EVENT;
    }

  public:
    double xPos, yPos;
};

struct MouseButtonPressEvent : public IEvent {
  public:
    inline MouseButtonPressEvent(const MouseCode pressedButton)
        : button(pressedButton) {}

    inline EventType getType() const override {
        return EventType::MOUSE_BUTTON_PRESS_EVENT;
    }

  public:
    MouseCode button;
};

struct MouseButtonReleaseEvent : public IEvent {
  public:
    inline MouseButtonReleaseEvent(const MouseCode releasedButton)
        : button(releasedButton) {}

    inline EventType getType() const override {
        return EventType::MOUSE_BUTTON_RELEASE_EVENT;
    }

  public:
    MouseCode button;
};

struct MouseScrollEvent : public IEvent {
  public:
    inline MouseScrollEvent(const double xScrollOffset,
                            const double yScrollOffset)
        : xOffset(xScrollOffset), yOffset(yScrollOffset) {}

    inline EventType getType() const override {
        return EventType::MOUSE_SCROLL_EVENT;
    }

  public:
    double xOffset, yOffset;
};

} // namespace Engine
