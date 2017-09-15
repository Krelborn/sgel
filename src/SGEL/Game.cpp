//
//  Game.cpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#include "SGEL/Game.hpp"

#include <memory>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "imgui.h"
#include "imgui-SFML.h"

#include "SGEL/Paths.hpp"

using namespace sgel;

using std::runtime_error;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

using rapidjson::Document;
using rapidjson::FileReadStream;

using sf::Clock;
using sf::RenderTarget;
using sf::Time;
using sf::Vector2i;

namespace
{
    /**
     *  The Game singleton reference
     *
     *  This will be set from the constructor when the first game object is
     *  created. It may be accessed through the GetGame() class method.
     */
    Game *sharedGame = nullptr;
    
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
 *  Create the game instance
 */
Game::Game()
{
    if (sharedGame == nullptr)
    {
        sharedGame = this;
    }
}

/**
 *  Destructor, clean everything up including the singleton reference
 */
Game::~Game()
{
    ImGui::SFML::Shutdown();
    
    if (sharedGame == this)
    {
        sharedGame = nullptr;
    }
}

/**
 *  @return The shared game singeton instance
 */
Game& Game::GetGame()
{
    return *sharedGame;
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
 *  @return The time since the last frame started
 */
Time Game::GetFrameDelta() const
{
    return frameDelta;
}

#pragma mark - Game Loop

/**
 *  Run the game
 */
void Game::Run()
{
    Initialize();
    
    Clock frameTimer;
    frameDelta = Time::Zero;

    
    // Start the game loop
    while (window.isOpen())
    {
        frameDelta = frameTimer.restart();
        
        ProcessAllEvents();
        
        Update();
        
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

    // Setup the immediate mode GUI
    ImGui::SFML::Init(window);
    auto &imguiIO = ImGui::GetIO();
    imguiIO.FontGlobalScale = 2.0;
    
    GetQuitEvent().Connect(this, &Game::Quit);
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
        BroadcastEvent(event);
        
        ImGui::SFML::ProcessEvent(event);
    }
}

/**
 *  Terminate the game gracefully
 */
void Game::Quit()
{
    window.close();
}

/**
 *  Process the current frame's update
 */
void Game::Update()
{
    ImGui::SFML::Update(window, GetFrameDelta());

    GetUpdateEvent().Emit();
}

/**
 *  Draw the current frame
 */
void Game::Draw()
{
    // Clear screen
    window.clear();
    
    // Actually call out to the draw handlers
    GetDrawEvent().Emit(window);
    
    ImGui::SFML::Render(window);
    
    // Update the window
    window.display();
}
