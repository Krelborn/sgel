//
//  Game.hpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#pragma once

#include <string>
#include <SFML/Graphics.hpp>

namespace sgel
{
    class Game
    {
    public:
                
        void            SetName(const std::string& inName);
        std::string     GetName() const;
        
        void            Run();
        
    private:
        
        void            Initialize();
        
        void            ProcessAllEvents();
        
        void            Draw();
        
    private:
        
        std::string         name;
        sf::RenderWindow    window;
    };
}
