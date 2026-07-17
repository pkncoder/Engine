#pragma once

#include <string>

namespace Engine {

enum struct EventType {

    // Key events
    KEY_PRESS_EVENT,
    KEY_RELEASE_EVENT,
    KEY_REPEAT_EVENT,

    // Mouse events
    MOUSE_BUTTON_PRESS_EVENT,
    MOUSE_BUTTON_RELEASE_EVENT,
    MOUSE_MOVE_EVENT,
    MOUSE_SCROLL_EVENT,

    // Renderer events
    NEW_FRAME_RENDERED_EVENT,

    // Window events
    WINDOW_RESIZE_EVENT
};

// std::toString extention for Event type
inline std::string to_string(const EventType type) {
    switch (type) {
    case EventType::KEY_PRESS_EVENT:
        return "KEY_PRESS_EVENT";
    case EventType::KEY_RELEASE_EVENT:
        return "KEY_RELEASE_EVENT";
    case EventType::KEY_REPEAT_EVENT:
        return "KEY_REPEAT_EVENT";
    case EventType::MOUSE_BUTTON_PRESS_EVENT:
        return "MOUSE_BUTTON_PRESS_EVENT";
    case EventType::MOUSE_BUTTON_RELEASE_EVENT:
        return "MOUSE_BUTTON_RELEASE_EVENT";
    case EventType::MOUSE_MOVE_EVENT:
        return "MOUSE_MOVE_EVENT";
    case EventType::MOUSE_SCROLL_EVENT:
        return "MOUSE_SCROLL_EVENT";
    case EventType::NEW_FRAME_RENDERED_EVENT:
        return "NEW_FRAME_RENDERED_EVENT";
    case EventType::WINDOW_RESIZE_EVENT:
        return "WINDOW_RESIZE_EVENT";
    default:
        return "ERROR - UNKNOWN_EVENT_TYPE";
    }
}

// Concatentation opporators for Event type
inline std::string operator+(const std::string &lhs, const EventType rhs) {
    return lhs + to_string(rhs);
}

// Event base class
struct IEvent {
  public:
    // Deconstructor
    virtual ~IEvent() = default;

    // Event type
    virtual EventType getType() const = 0;

    // Consumtion logic
    inline void consume() { consumed = true; }
    inline bool isConsumed() const { return consumed; }

  private:
    // If an event is marked consumed, it will stop processing
    bool consumed = false;
};

} // namespace Engine
