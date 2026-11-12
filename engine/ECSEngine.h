#pragma once

#include <vector>

#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "EntityManager.h"
#include "InputComponent.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "ScoreComponent.h"
#include "SoundManager.h"
#include "SpawnComponent.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace ECSEngine {

template <typename... Components>
class EngineView {
public:
    virtual ~EngineView() = default;
    virtual SoundManager& GetSoundManager() = 0;
    virtual SpriteManager& GetSpriteManager() = 0;
    virtual EntityManager<Components...>& GetEntityManager() = 0;
    virtual WindowManager& GetWindowManager() = 0;
};

template <typename... Components>
class ECSEngine : public EngineView<Components...> {
public:
    ECSEngine(unsigned int width, unsigned int height, const std::string& name);
    ~ECSEngine();

    void Run();

    SoundManager& GetSoundManager() override { return mSoundManager; }
    SpriteManager& GetSpriteManager() override { return mSpriteManager; }
    EntityManager<Components...>& GetEntityManager() override { return mEntityManager; }
    WindowManager& GetWindowManager() override { return mWindowManager; }
    SystemManager<Components...>& GetSystemManager() { return mSystemManager; }

private:
    bool ProcessEvents();
    void DebugDraw();

    EntityManager<Components...> mEntityManager;
    SystemManager<Components...> mSystemManager;
    SpriteManager mSpriteManager;
    SoundManager mSoundManager;
    WindowManager mWindowManager;

    // For debugging - pause and step the engine
    bool running, step;
    int debug;
};

template <typename... Components>
ECSEngine<Components...>::ECSEngine(unsigned int width, unsigned int height,
                                    const std::string& name)
    : mWindowManager(width, height, name), running(true), step(false), debug(0) {}

template <typename... Components>
ECSEngine<Components...>::~ECSEngine() {}

/// @brief The main game engine loop, processes events like key inputs, calls Run() on the systemManager, and displays the window. 
///        Has the option to toggle debug mode or pause the current loop as well. 
/// @tparam ...Components 
template <typename... Components>
void ECSEngine<Components...>::Run()
{
    while (mWindowManager.GetWindow()->isOpen() && running)
    {
        if (!ProcessEvents())
        {
            break;
        }

        if (!step)
        {
            mSystemManager.Run(this);
        }

        mWindowManager.GetWindow()->clear();

        if (debug != 0)
        {
            DebugDraw();
        }

        mWindowManager.GetWindow()->display();
    }
}

/// @brief Gets key input and updates the inputComponent's bitmaps for keys pressed and released this frame. 
///        Can toggle debug mode with F3 and pause the game loop with F4. 
/// @tparam ...Components 
/// @return Returns a bool. If true, continue the game loop. If false, the window was closed and the game will stop. 
template <typename... Components>
bool ECSEngine<Components...>::ProcessEvents() {
    while(const std::optional<sf::Event> event = mWindowManager.GetWindow()->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            mWindowManager.GetWindow()->close();
            return false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F3))
        {
            if (debug == 0) { debug = 1; }
            else { debug = 0; }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F4))
        {
            step = !step;
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            auto& componentStorage = mEntityManager.template GetComponentStorage<InputComponent>();
            for (int i = 0; i < componentStorage.mStorage.size(); i++)
            {
                if (componentStorage.mValid[i] != 0)
                {
                    auto storage = componentStorage.mStorage[i];
                    storage.lastKeydown = storage.keydown;
                    if (event->is<sf::Event::KeyPressed>())
                    {
                        storage.keydown.set((size_t)keyPressed->scancode, true);
                    }
                    else if (event->is<sf::Event::KeyReleased>())
                    {
                        storage.keydown.set((size_t)keyPressed->scancode, false);
                    }
                }
            }
        }
        else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
        {
            auto& componentStorage = mEntityManager.template GetComponentStorage<InputComponent>();
            for (int i = 0; i < componentStorage.mStorage.size(); i++)
            {
                if (componentStorage.mValid[i] != 0)
                {
                    auto storage = componentStorage.mStorage[i];
                    storage.lastKeydown = storage.keydown;
                    if (event->is<sf::Event::KeyPressed>())
                    {
                        storage.keydown.set((size_t)keyReleased->scancode, true);
                    }
                    else if (event->is<sf::Event::KeyReleased>())
                    {
                        storage.keydown.set((size_t)keyReleased->scancode, false);
                    }
                }
            }
        }
    }
    return true;
}

template <typename... Components>
void ECSEngine<Components...>::DebugDraw() {}

}  // namespace ECSEngine
