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
    Document LoadJSON(const string& name)
    {
        string path = Paths::GetResourcePath() + name + ".json";
        
        FILE *file = fopen(path.c_str(), "r");
        if (!file)
        {
            throw runtime_error("LoadConfig: Could not open file");
        }
        
        char readBuffer[65536];
        FileReadStream is(file, readBuffer, sizeof(readBuffer));
        
        Document json;
        json.ParseStream(is);
        
        fclose(file);
    
        return json;
    }
}

/**
 *  Create the game instance
 */
Game::Game() :
    editorVisible(false),
    fullScreen(false)
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
 *  Toggle the game between full screen and windowed modes
 *
 *  @param  inFullScreen Game will be in a full screen window if true
 */
void Game::SetFullScreen(bool inFullScreen)
{
    fullScreen = inFullScreen;
}

/**
 *  @return true if the game is running in full screen mode
 */
bool Game::IsFullScreen() const
{
    return fullScreen;
}

/**
 *  Set the video mode used to display the game
 *
 *  @param  inVideoMode The new video mode (Must be a valid mode)
 */
void Game::SetVideoMode(const sf::VideoMode& inVideoMode)
{
    videoMode = inVideoMode;
}

/**
 *  @return The current video mode for the game
 */
const sf::VideoMode& Game::GetVideoMode() const
{
    return videoMode;
}


/**
 *  Show/hide the editor interface
 *
 *  @param  visible If true the immediate mode editor GUI will be displayed
 */
void Game::SetEditorVisible(bool visible)
{
    editorVisible = visible;
}

/**
 *  @return true if the immediate mode editor is visible in the game window
 */
bool Game::IsEditorVisible() const
{
    return editorVisible;
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
        bool savedFullScreen = IsFullScreen();
        sf::VideoMode savedVideoMode = GetVideoMode();

        frameDelta = frameTimer.restart();
        
        ProcessAllEvents();
        
        Update();
        
        Draw();

        if ((savedFullScreen != IsFullScreen()) || (savedVideoMode != GetVideoMode()))
        {
            uint32_t style = sf::Style::Titlebar | sf::Style::Close;
            if (IsFullScreen())
            {
                style = sf::Style::Fullscreen;
            }

            window.create(videoMode, GetName(), style);
        }
    }
}

/**
 *  Do all the game setup before the game loop starts
 */
void Game::Initialize()
{
    Document config(std::move(LoadJSON("config")));
    
    Vector2i windowSize(640, 480);
    if (config.HasMember("window"))
    {
        videoMode.width = config["window"]["size"]["width"].GetInt();
        videoMode.height = config["window"]["size"]["height"].GetInt();
    }
    
    if (config.HasMember("name"))
    {
        SetName(config["name"].GetString());
    }

    // Create the main window
    window.create(videoMode, GetName(), sf::Style::Titlebar | sf::Style::Close);

    if (config.HasMember("editor"))
    {
        if (config["editor"].HasMember("visible"))
        {
            editorVisible = config["editor"]["visible"].GetBool();
        }
    }

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

        // Give events to the editor if it's visible
        if (IsEditorVisible())
        {
            ImGui::SFML::ProcessEvent(event);
        }
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
    // If the editor is visible, give it an update. GUI shouldn't be drawn if this isn't being
    // called!
    if (IsEditorVisible())
    {
        ImGui::SFML::Update(window, GetFrameDelta());
    }

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

    // Editor GUI is drawn on top of the game elements
    if (IsEditorVisible())
    {
        ImGui::SFML::Render(window);
    }

    // Update the window
    window.display();
}
