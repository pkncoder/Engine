#pragma once

#include "IEvent.h"
#include <glm/fwd.hpp>

namespace Engine {

using KeyCode = glm::uint16_t;
namespace Key {

// From glfw3.h
enum : KeyCode {

    // Alpha
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,

    // Numeric (didital)
    D0 = 48,
    D1 = 49,
    D2 = 50,
    D3 = 51,
    D4 = 52,
    D5 = 53,
    D6 = 54,
    D7 = 55,
    D8 = 56,
    D9 = 57,

    // Other keys
    ESCAPE = 256,
    PAUSE = 284,
    DELETE_KEY = 261,
    TAB = 258,
    ENTER = 257,
    BACKSPACE = 259,
    SPACE = 32,

    // Arrow keys
    UP = 265,
    DOWN = 264,
    LEFT = 263,
    RIGHT = 262,

    // Function keys
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,

    // Shift
    LEFT_SHIFT = 340,
    RIGHT_SHIFT = 344,

    // Control (ctrl)
    LEFT_CONTROL = 341,
    RIGHT_CONTROL = 345,

    // Alt
    LEFT_ALT = 342,
    RIGHT_ALT = 346,

    // Super (windows/command)
    LEFT_SUPER = 343,
    RIGHT_SUPER = 347,
};

} // namespace Key

// First press action
struct KeyPressEvent : public IEvent {
  public:
    // Takes the key pressed + the state of modifier keys
    inline KeyPressEvent(const KeyCode key, const bool ctrl, const bool shift,
                         const bool alt, const bool super)
        : key(key), keyCtrl(ctrl), keyShift(shift), keyAlt(alt),
          keySuper(super) {}

    // Event type return
    inline EventType getType() const override {
        return EventType::KEY_PRESS_EVENT;
    }

  public:
    // Key + modifier keys
    KeyCode key;
    bool keyCtrl, keyShift, keyAlt, keySuper;
};

// On key let go
struct KeyReleaseEvent : public IEvent {
  public:
    // Takes the released key + the state of modifier keys
    inline KeyReleaseEvent(const KeyCode k, const bool ctrl, const bool shift,
                           const bool alt, const bool super)
        : key(k), keyCtrl(ctrl), keyShift(shift), keyAlt(alt), keySuper(super) {
    }

    // Event type return
    inline EventType getType() const override {
        return EventType::KEY_RELEASE_EVENT;
    }

  public:
    // Key + modifier keys
    KeyCode key;
    bool keyCtrl, keyShift, keyAlt, keySuper;
};

// On key held - includes a buffer between KEY_PRESS_EVENT && KEY_REPEAT_EVENT
struct KeyRepeatEvent : public IEvent {
  public:
    // Takes the released key + the state of modifier keys
    inline KeyRepeatEvent(const KeyCode k, const bool ctrl, const bool shift,
                          const bool alt, const bool super)
        : key(k), keyCtrl(ctrl), keyShift(shift), keyAlt(alt), keySuper(super) {
    }

    // Event type return
    inline EventType getType() const override {
        return EventType::KEY_REPEAT_EVENT;
    }

  public:
    // Key + modifier keys
    KeyCode key;
    bool keyCtrl, keyShift, keyAlt, keySuper;
};
} // namespace Engine
