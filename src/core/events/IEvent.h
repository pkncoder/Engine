#pragma once

namespace Engine {

enum struct EventType {

    KEY_PRESS_EVENT,
    KEY_RELEASE_EVENT,
    KEY_REPEAT_EVENT,

    MOUSE_MOVE_EVENT,
    MOUSE_BUTTON_PRESS_EVENT,
    MOUSE_BUTTON_RELEASE_EVENT,
    MOUSE_SCROLL_EVENT,

    NEW_FRAME_RENDERED_EVENT,

    WINDOW_RESIZE_EVENT,

    EVENT_COUNT
};

struct IEvent {
    virtual ~IEvent() = default;

    virtual EventType getType() const = 0;

    inline void consume() { consumed = true; }
    inline bool isConsumed() const { return consumed; }

  private:
    bool consumed = false;
};

} // namespace Engine
