//
//  EventBroadcaster.cpp
//
//  Copyright © 2017 Compiled Creations Ltd. All rights reserved.
//

#include "EventBroadcaster.hpp"

using namespace sgel;

using sf::Event;
using sf::RenderTarget;

using KeyEvent = sf::Event::KeyEvent;
using TextEvent = sf::Event::TextEvent;


/**
 *  Empty virtual destructor for base class
 */
EventBroadcaster::~EventBroadcaster()
{
}

/**
 *  Dispatch the given event using our signals
 */
void EventBroadcaster::BroadcastEvent(const Event& event)
{
    GetGenericEvent().Emit(event);

    switch (event.type)
    {
        case Event::Closed:
            GetQuitEvent().Emit();
            break;

        case Event::KeyPressed:
            GetKeyDownEvent().Emit(event.key);
            break;

        case Event::KeyReleased:
            GetKeyUpEvent().Emit(event.key);
            break;

        case Event::TextEntered:
            GetTextEvent().Emit(event.text);
            break;

        default:
            break;
    }
}

Signal<>& EventBroadcaster::GetUpdateEvent()
{
    return updateEvent;
}

Signal<RenderTarget&>& EventBroadcaster::GetDrawEvent()
{
    return drawEvent;
}

Signal<>& EventBroadcaster::GetQuitEvent()
{
    return quitEvent;
}

Signal<const Event&>& EventBroadcaster::GetGenericEvent()
{
    return genericEvent;
}

Signal<const KeyEvent&>& EventBroadcaster::GetKeyDownEvent()
{
    return keyDownEvent;
}

Signal<const KeyEvent&>& EventBroadcaster::GetKeyUpEvent()
{
    return keyUpEvent;
}

Signal<const TextEvent&>& EventBroadcaster::GetTextEvent()
{
    return textEvent;
}

