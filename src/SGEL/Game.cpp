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
using std::vector;
using std::weak_ptr;

using rapidjson::Document;
using rapidjson::FileReadStream;

using sf::Clock;
using sf::RenderTarget;
using sf::Time;
using sf::Vector2i;

using KeyEvent = sf::Event::KeyEvent;
using TextEvent = sf::Event::TextEvent;

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
    
    /**
     *  Generic helper for binding event handlers to binding tokens
     */
    template <class HandlerT>
    shared_ptr<HandlerT> AddEventHandlerToContainer(const HandlerT& handler, vector<weak_ptr<HandlerT>> &inOutContainer)
    {
        shared_ptr<HandlerT> binding(new HandlerT(std::move(handler)));
        inOutContainer.push_back(binding);
        return binding;
    }
    
    /**
     *  Invoke handlers for events without arguments
     */
    void DispatchEvent(const vector<weak_ptr<std::function<void(void)>>>& container)
    {        
        for (auto currentBinding : container)
        {
            shared_ptr<std::function<void(void)>> lockedBinding = currentBinding.lock();
            if (lockedBinding)
            {
                (*lockedBinding)();
            }
        }
    }

    /**
     *  Invoke handlers for event with a single argument
     */
    template <typename EventT>
    void DispatchEvent(EventT event, const vector<weak_ptr<std::function<void(EventT)>>>& container)
    {
        for (auto currentBinding : container)
        {
            shared_ptr<std::function<void(EventT)>> lockedBinding = currentBinding.lock();
            if (lockedBinding)
            {
                (*lockedBinding)(event);
            }
        }
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
        DispatchEvent<const Event&>(event, genericEventHandlers);
        
        switch (event.type)
        {
            case Event::Closed:
                // Close window: exit
                Quit();
                break;
                
            case Event::KeyPressed:                
                DispatchEvent<const KeyEvent&>(event.key, keyDownEventHandlers);
                break;
                
            case Event::KeyReleased:
                DispatchEvent<const KeyEvent&>(event.key, keyUpEventHandlers);
                break;
                
            case Event::TextEntered:
                DispatchEvent<const TextEvent&>(event.text, textEventHandlers);
                break;
                
            default:
                break;
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
    DispatchEvent(updateEventHandlers);
}

/**
 *  Draw the current frame
 */
void Game::Draw()
{
    // Clear screen
    window.clear();
    
    // Actually call out to the draw handlers
    DispatchEvent<RenderTarget&>(window, drawEventHandlers);
    
    // Update the window
    window.display();
}

#pragma mark - Event Management

/**
 *  Bind a function that will be called for every update phase
 *
 *  @param  handler Function for processing updates
 *
 *  @return A binding token used by the caller to keep the handler alive
 */
UpdateEventHandlerBinding Game::AddUpdateEventHandler(const UpdateEventHandler& handler)
{
    return AddEventHandlerToContainer(handler, updateEventHandlers);
}

/**
 *  Bind a function that will be called for every update phase
 *
 *  @param  handler Function for processing updates
 *
 *  @return A binding token used by the caller to keep the handler alive
 */
DrawEventHandlerBinding Game::AddDrawEventHandler(const DrawEventHandler& handler)
{
    return AddEventHandlerToContainer(handler, drawEventHandlers);
}

/**
 *  Bind a function that will be called for every event
 *
 *  @param  handler Function object taking any Event as an argument
 *
 *  @return A binding token used by the caller to keep the handler alive
 */
GenericEventHandlerBinding Game::AddGenericEventHandler(const GenericEventHandler& handler)
{
    return AddEventHandlerToContainer(handler, genericEventHandlers);
}

/**
 *  Bind a function to key down event
 *
 *  The function will be called for every key down event. The caller must hold onto the returned
 *  binding object otherwise the handler will be released.
 *
 *  @param  handler Function object taking a KeyEvent as an argument
 *
 *  @return A binding token used by the caller to keep the handler alive
 */
KeyEventHandlerBinding Game::AddKeyDownEventHandler(const KeyEventHandler& handler)
{
    return AddEventHandlerToContainer(handler, keyDownEventHandlers);
}

/**
 *  Bind a function to key up event
 *
 *  The function will be called for every key up event. The caller must hold onto the returned
 *  binding object otherwise the handler will be released.
 *
 *  @param  handler Function object taking a KeyEvent as an argument
 *
 *  @return A binding token used by the caller to keep the handler alive
 */
KeyEventHandlerBinding Game::AddKeyUpEventHandler(const KeyEventHandler& handler)
{
    return AddEventHandlerToContainer(handler, keyUpEventHandlers);
}

/**
 *  Bind a function to text input event
 *
 *  The function will be called for every text input event. The caller must hold onto the returned
 *  binding object otherwise the handler will be released.
 *
 *  @param  handler Function object taking a TextEvent as an argument
 *
 *  @return A binding token used by the caller to keep the handler alive
 */
TextEventHandlerBinding Game::AddTextEventHandler(const TextEventHandler& handler)
{
    return AddEventHandlerToContainer(handler, textEventHandlers);
}


