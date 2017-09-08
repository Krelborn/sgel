//
//  Game.hpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "SGEL/Events.hpp"

namespace sgel
{
    // Types and bindings for event handlers
    DECLARE_EVENT_HANDLER1(const sf::Event::Event&, GenericEvent);
    DECLARE_EVENT_HANDLER1(const sf::Event::KeyEvent&, KeyEvent);
    DECLARE_EVENT_HANDLER1(const sf::Event::TextEvent&, TextEvent);
    DECLARE_EVENT_HANDLER1(sf::RenderTarget&, DrawEvent);
    DECLARE_EVENT_HANDLER0(UpdateEvent);
    
    class Game
    {
    public:
        
        Game();
        ~Game();
        
        static Game&                GetGame();
        
        void                        SetName(const std::string& inName);
        std::string                 GetName() const;
        
        sf::Time                    GetFrameDelta() const;
        
        void                        Run();
        
        void                        Quit();
        
        UpdateEventHandlerBinding   AddUpdateEventHandler(const UpdateEventHandler& handler);
        DrawEventHandlerBinding     AddDrawEventHandler(const DrawEventHandler& handler);
        GenericEventHandlerBinding  AddGenericEventHandler(const GenericEventHandler& handler);
        KeyEventHandlerBinding      AddKeyDownEventHandler(const KeyEventHandler& handler);
        KeyEventHandlerBinding      AddKeyUpEventHandler(const KeyEventHandler& handler);
        TextEventHandlerBinding     AddTextEventHandler(const TextEventHandler& handler);
        
    private:
        
        void                    Initialize();
        
        void                    ProcessAllEvents();
        
        void                    Update();
        void                    Draw();
        
    private:
        
        std::string         name;
        sf::RenderWindow    window;
        sf::Time            frameDelta;
        
        std::vector<WeakUpdateEventHandlerBinding>  updateEventHandlers;
        std::vector<WeakDrawEventHandlerBinding>    drawEventHandlers;
        std::vector<WeakGenericEventHandlerBinding> genericEventHandlers;
        std::vector<WeakKeyEventHandlerBinding>     keyDownEventHandlers;
        std::vector<WeakKeyEventHandlerBinding>     keyUpEventHandlers;
        std::vector<WeakTextEventHandlerBinding>    textEventHandlers;

    };
}
