#pragma once

#include "IEvent.h"

namespace Engine {

// TODO: create the frame image2d
struct NewFrameRenderedEvent : public IEvent {
  public:
    // std::shared_ptr<IImage2D> frame;

    inline NewFrameRenderedEvent() {}

    inline EventType getType() const override {
        return EventType::NEW_FRAME_RENDERED_EVENT;
    }
};

} // namespace Engine
