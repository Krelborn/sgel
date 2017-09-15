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

#include "SGEL/EventBroadcaster.hpp"

namespace sgel
{
    class Game : public EventBroadcaster
    {
    public:
        
        Game();
        ~Game() override;
        
        static Game&            GetGame();
        
        void                    SetName(const std::string& inName);
        std::string             GetName() const;

        void                    SetFullScreen(bool inFullScreen);
        bool                    IsFullScreen() const;

        void                    SetVideoMode(const sf::VideoMode& inVideoMode);
        const sf::VideoMode&    GetVideoMode() const;

        void                    SetEditorVisible(bool visible);
        bool                    IsEditorVisible() const;
        
        sf::Time                GetFrameDelta() const;
        
        void                    Run();
        
        void                    Quit();

    private:
        
        void                    Initialize();
        
        void                    ProcessAllEvents();
        
        void                    Update();
        void                    Draw();
        
    private:
        
        std::string         name;
        sf::RenderWindow    window;
        sf::Time            frameDelta;

        bool                editorVisible;

        bool                fullScreen;
        sf::VideoMode       videoMode;
    };
}
