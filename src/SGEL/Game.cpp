//
//  Game.cpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#include "Game.hpp"

#include <memory>
#include <stdexcept>

#include <rapidjson/Document.h>
#include <rapidjson/filereadstream.h>

#include "Paths.hpp"

using namespace sgel;

using std::runtime_error;
using std::shared_ptr;
using std::string;

using rapidjson::Document;
using rapidjson::FileReadStream;

using sf::Vector2i;

namespace
{
    /**
     *  Load a JSON document from the resources with the given name
     *
     *  @param  name The JSON file name without file extension
     *
     *  @return A JSON document
     */
    shared_ptr<Document> LoadJSON(const string& name)
    {
        string path = Paths::GetResourcePath() + name + ".json";
        
        FILE *file = fopen(path.c_str(), "r");
        if (!file)
        {
            throw runtime_error("LoadConfig: Could not open file");
        }
        
        char readBuffer[65536];
        FileReadStream is(file, readBuffer, sizeof(readBuffer));
        
        shared_ptr<Document> json = std::make_shared<Document>();
        json->ParseStream(is);
        
        fclose(file);
    
        return json;
    }
}

/**
 *  Set the name used for the window title etc
 *
 *  @param  inName The new name
 */
void Game::SetName(const std::string& inName)
{
    name = inName;
}

/**
 *  @return The name of the game
 */
string Game::GetName() const
{
    return name;
}

/**
 *  Run the game
 */
void Game::Run()
{
    Initialize();
    
    // Start the game loop
    while (window.isOpen())
    {
        ProcessAllEvents();
        
        Draw();
    }
}

/**
 *  Do all the game setup before the game loop starts
 */
void Game::Initialize()
{
    shared_ptr<Document> config = LoadJSON("config");
    
    Vector2i windowSize(640, 480);
    if (config->HasMember("window"))
    {
        windowSize.x = (*config)["window"]["size"]["width"].GetInt();
        windowSize.y = (*config)["window"]["size"]["height"].GetInt();
    }
    
    if (config->HasMember("name"))
    {
        SetName((*config)["name"].GetString());
    }

    // Create the main window
    window.create(sf::VideoMode(windowSize.x,windowSize.y), GetName(), sf::Style::Titlebar | sf::Style::Close);
}

/**
 *  Process all events in the queue until empty
 */
void Game::ProcessAllEvents()
{
    using sf::Event;
    
    Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case Event::Closed:
                // Close window: exit
                window.close();
                break;
                
            case Event::KeyPressed:
                // Escape pressed: exit
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
                
            default:
                break;
        }
    }
}

/**
 *  Draw the current frame
 */
void Game::Draw()
{
    // Clear screen
    window.clear();
    
    // TODO: Actually call out to the scene graph or something
    
    // Update the window
    window.display();
}


