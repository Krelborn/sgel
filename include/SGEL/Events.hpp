//
//  Events.hpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#pragma once

/**
 *  Macro defines event standard handler types and their respective binding objects
 * for use with the Game class event routing system.
 */
#define DECLARE_EVENT_HANDLER0(eventName)               typedef std::function<void(void)> eventName ## Handler; \
                                                        typedef std::shared_ptr<eventName ## Handler> eventName ## HandlerBinding; \
                                                        typedef std::weak_ptr<eventName ## Handler> Weak ## eventName ## HandlerBinding;

#define DECLARE_EVENT_HANDLER1(eventType, eventName)    typedef std::function<void(eventType)> eventName ## Handler; \
                                                        typedef std::shared_ptr<eventName ## Handler> eventName ## HandlerBinding; \
                                                        typedef std::weak_ptr<eventName ## Handler> Weak ## eventName ## HandlerBinding;
