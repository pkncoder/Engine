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
    double xPos, yPos;

    inline MouseMoveEvent(double xMousePos, double yMousePos)
        : xPos(xMousePos), yPos(yMousePos) {}

    inline EventType getType() const override {
        return EventType::MOUSE_MOVE_EVENT;
    }
};

struct MouseButtonPressEvent : public IEvent {
    MouseCode button;

    inline MouseButtonPressEvent(MouseCode pressedButton)
        : button(pressedButton) {}

    inline EventType getType() const override {
        return EventType::MOUSE_BUTTON_PRESS_EVENT;
    }
};

struct MouseButtonReleaseEvent : public IEvent {
    MouseCode button;

    inline MouseButtonReleaseEvent(MouseCode releasedButton)
        : button(releasedButton) {}

    inline EventType getType() const override {
        return EventType::MOUSE_BUTTON_RELEASE_EVENT;
    }
};

struct MouseScrollEvent : public IEvent {
    double xOffset, yOffset;

    inline MouseScrollEvent(double xScrollOffset, double yScrollOffset)
        : xOffset(xScrollOffset), yOffset(yScrollOffset) {}

    inline EventType getType() const override {
        return EventType::MOUSE_SCROLL_EVENT;
    }
};

} // namespace Engine
