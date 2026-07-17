#pragma once

#include "IEvent.h"

namespace Engine {

// TODO: create the frame image2d
struct NewFrameRenderedEvent : public IEvent {
  public:
    // Takes in the IImage2D for the frame image
    inline NewFrameRenderedEvent() {}

    // Event type return
    inline EventType getType() const override {
        return EventType::NEW_FRAME_RENDERED_EVENT;
    }

  public:
    // std::shared_ptr<IImage2D> frame;
};

} // namespace Engine
