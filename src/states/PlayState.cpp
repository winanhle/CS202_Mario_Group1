#include "PlayState.h"
#include "PlayState.h"
#include "../world/GameWorld.h"
#include "../entities/player/Mario.h"
#include "../entities/MapManager.h"
#include "../entities/camera/CameraManager.h"
#include "../entities/player/PlayerManager.h"
#include "../entities/EnemyManager.h"
#include "../entities/ItemManager.h"
#include "../ui/HUDManager.h"
#include "../ui/SaveManager.h"
#include <SFML/Graphics.hpp>

PlayState::PlayState()
    : m_gameWorld(std::make_unique<GameWorld>())
{
    // ==================== MODULE INITIALIZATION ====================
    // This is where all team modules are connected to the GameWorld
    // Each team member replaces the stub managers with their implementations

    // TODO: Quynh Anh - INTEGRATION LEAD
    // These manager instances need to be created by their respective developers
    // and set on the GameWorld. For now, we use stub implementations.

    // Module 1: Le Tran - Character/Player
    // TODO: Le Tran - Replace with your PlayerManager implementation
    m_gameWorld->setMapManager(std::make_shared<MapManager>());
    m_gameWorld->setCameraManager(std::make_shared<CameraManager>());
    m_gameWorld->setPlayerManager(std::make_shared<Mario>());

    // Module 2: Dinh Anh - Enemies
    // TODO: Dinh Anh - Replace with your EnemyManager implementation
    m_gameWorld->setEnemyManager(std::make_shared<EnemyManager>());

    // Module 3: Dinh Anh - Items
    // TODO: Dinh Anh - Replace with your ItemManager implementation
    m_gameWorld->setItemManager(std::make_shared<ItemManager>());

    // Module 4: Nguyen Phuc - UI/HUD
    // TODO: Nguyen Phuc - Replace with your HUDManager implementation
    m_gameWorld->setHUDManager(std::make_shared<HUDManager>());

    // Module 5: Nguyen Phuc - Save/Load
    // TODO: Nguyen Phuc - Replace with your SaveManager implementation
    m_gameWorld->setSaveManager(std::make_shared<SaveManager>());

    // Initialize the game world
    m_gameWorld->initialize();
}

PlayState::~PlayState()
{
    // GameWorld will be automatically cleaned up via unique_ptr
}

void PlayState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::P)
        {
            // TODO: Quynh Anh - Transition to PauseState via StateManager
        }
    }

    // Delegate input to GameWorld
    m_gameWorld->handleInput(event);
}

void PlayState::update(float deltaTime)
{
    // Delegate all game logic to GameWorld
    m_gameWorld->update(deltaTime);
}

void PlayState::render(sf::RenderWindow& window) const
{
    // Clear with black background
    window.clear(sf::Color::Black);

    // Render game world and all its systems
    m_gameWorld->render(window);
}
