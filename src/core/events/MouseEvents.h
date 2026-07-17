#pragma once

#include "IEvent.h"

#include <glm/fwd.hpp>

namespace Engine {

using MouseCode = glm::uint16_t;

namespace Mouse {

// From glfw3.h
enum : MouseCode {

    // Pure mouse button clicks
    MOUSE_BUTTON_1 = 0,
    MOUSE_BUTTON_2 = 1,
    MOUSE_BUTTON_3 = 2,
    MOUSE_BUTTON_4 = 3,
    MOUSE_BUTTON_5 = 4,
    MOUSE_BUTTON_6 = 5,
    MOUSE_BUTTON_7 = 6,
    MOUSE_BUTTON_8 = 7,

    // Quick access for the main mouse clicks
    MOUSE_LEFT = MOUSE_BUTTON_1,
    MOUSE_RIGHT = MOUSE_BUTTON_2,
    MOUSE_MIDDLE = MOUSE_BUTTON_3
};

} // namespace Mouse

struct MouseButtonPressEvent : public IEvent {
  public:
    // Pressed mouse button
    inline MouseButtonPressEvent(const MouseCode pressedButton)
        : button(pressedButton) {}

    // Event type return
    inline EventType getType() const override {
        return EventType::MOUSE_BUTTON_PRESS_EVENT;
    }

  public:
    // Pressed button
    MouseCode button;
};

// On the unclick of a mouse button
struct MouseButtonReleaseEvent : public IEvent {
  public:
    // Released mouse button
    inline MouseButtonReleaseEvent(const MouseCode releasedButton)
        : button(releasedButton) {}

    // Event type return
    inline EventType getType() const override {
        return EventType::MOUSE_BUTTON_RELEASE_EVENT;
    }

  public:
    // Released button
    MouseCode button;
};

// On mouse move
struct MouseMoveEvent : public IEvent {
  public:
    // Mouse position
    inline MouseMoveEvent(const double xMousePos, const double yMousePos)
        : xPos(xMousePos), yPos(yMousePos) {}

    // Event type return
    inline EventType getType() const override {
        return EventType::MOUSE_MOVE_EVENT;
    }

  public:
    // x&y position
    double xPos, yPos;
};

// Scroll wheel change - WORKS WITH OFFSETS
struct MouseScrollEvent : public IEvent {
  public:
    // Takes the x & y offset of the scroll wheel - not the pure position
    inline MouseScrollEvent(const double xScrollOffset,
                            const double yScrollOffset)
        : xOffset(xScrollOffset), yOffset(yScrollOffset) {}

    // Event type return
    inline EventType getType() const override {
        return EventType::MOUSE_SCROLL_EVENT;
    }

  public:
    // x&y offset
    double xOffset, yOffset;
};

} // namespace Engine
