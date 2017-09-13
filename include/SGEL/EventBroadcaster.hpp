//
//  EventBroadcaster.h
//
//  Copyright © 2017 Compiled Creations Ltd. All rights reserved.
//

#pragma once

#include <SFML/Graphics.hpp>

#include "SGEL/Signal.hpp"

namespace sgel
{
    /**
     *  Class responsible for emitting signals for standard game events
     *
     *  Components will typical connect their methods to event signals. Classes may inherit from
     *  this to become broadcasters to their own objects in order to set up a chain of broadcasters
     *  by connecting their BroadcastEvent to the parent broadcasters generic event signal.
     */
    class EventBroadcaster
    {
    public:

        virtual ~EventBroadcaster();

        void                                    BroadcastEvent(const sf::Event& event);

        Signal<>&                               GetUpdateEvent();
        Signal<sf::RenderTarget&>&              GetDrawEvent();
        Signal<>&                               GetQuitEvent();

        Signal<const sf::Event&>&           	GetGenericEvent();
        Signal<const sf::Event::KeyEvent&>& 	GetKeyDownEvent();
        Signal<const sf::Event::KeyEvent&>&     GetKeyUpEvent();
        Signal<const sf::Event::TextEvent&>&    GetTextEvent();

    private:

        Signal<>                                updateEvent;
        Signal<sf::RenderTarget&>               drawEvent;
        Signal<>                                quitEvent;

        Signal<const sf::Event&>                genericEvent;
        Signal<const sf::Event::KeyEvent&>      keyDownEvent;
        Signal<const sf::Event::KeyEvent&>      keyUpEvent;
        Signal<const sf::Event::TextEvent&>     textEvent;
    };
}
