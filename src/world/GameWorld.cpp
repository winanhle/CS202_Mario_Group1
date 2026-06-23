#include "GameWorld.h"
#include "../interfaces/IPlayerManager.h"
#include "../interfaces/IEnemyManager.h"
#include "../interfaces/IItemManager.h"
#include "../interfaces/IHUDManager.h"
#include "../interfaces/ISaveManager.h"

GameWorld::GameWorld()
    : m_isInitialized(false)
{
}

void GameWorld::initialize()
{
    // TODO: Initialize player manager
    if (m_playerManager)
    {
        m_playerManager->initialize();
    }

    // TODO: Initialize enemy manager
    if (m_enemyManager)
    {
        m_enemyManager->initialize();
    }

    // TODO: Initialize item manager
    if (m_itemManager)
    {
        m_itemManager->initialize();
    }

    // TODO: Initialize HUD manager
    if (m_hudManager)
    {
        m_hudManager->initialize();
    }

    // TODO: Initialize save manager
    if (m_saveManager)
    {
        m_saveManager->initialize();
    }

    m_isInitialized = true;
}

void GameWorld::update(float deltaTime)
{
    if (!m_isInitialized)
        return;

    // TODO: Update enemy manager (should run first for predictability)
    if (m_enemyManager)
    {
        m_enemyManager->update(deltaTime);
    }

    // TODO: Update item manager
    if (m_itemManager)
    {
        m_itemManager->update(deltaTime);
    }

    // TODO: Update player manager (after enemies/items so collisions are detected)
    if (m_playerManager)
    {
        m_playerManager->update(deltaTime);
    }

    // TODO: Update HUD manager with current state
    if (m_hudManager)
    {
        m_hudManager->update(deltaTime);
    }
}

void GameWorld::render(sf::RenderWindow& window) const
{
    if (!m_isInitialized)
        return;

    // TODO: Render game world (background, tiles, etc.)
    // This will be extended by the Level/World module

    // TODO: Render enemies
    if (m_enemyManager)
    {
        m_enemyManager->render(window);
    }

    // TODO: Render items
    if (m_itemManager)
    {
        m_itemManager->render(window);
    }

    // TODO: Render player (on top of world)
    if (m_playerManager)
    {
        m_playerManager->render(window);
    }

    // TODO: Render HUD (on top of everything)
    if (m_hudManager)
    {
        m_hudManager->render(window);
    }
}

void GameWorld::handleInput(const sf::Event& event)
{
    if (!m_isInitialized)
        return;

    // TODO: Delegate input to player manager
    if (m_playerManager)
    {
        m_playerManager->handleInput(event);
    }

    // TODO: Delegate input to HUD manager (for menu interactions, etc.)
    if (m_hudManager)
    {
        m_hudManager->handleInput(event);
    }
}

// ==================== EXTENSION POINTS ====================

void GameWorld::setPlayerManager(std::shared_ptr<IPlayerManager> playerManager)
{
    m_playerManager = playerManager;
}

void GameWorld::setEnemyManager(std::shared_ptr<IEnemyManager> enemyManager)
{
    m_enemyManager = enemyManager;
}

void GameWorld::setItemManager(std::shared_ptr<IItemManager> itemManager)
{
    m_itemManager = itemManager;
}

void GameWorld::setHUDManager(std::shared_ptr<IHUDManager> hudManager)
{
    m_hudManager = hudManager;
}

void GameWorld::setSaveManager(std::shared_ptr<ISaveManager> saveManager)
{
    m_saveManager = saveManager;
}

// ==================== ACCESSORS ====================

IPlayerManager* GameWorld::getPlayerManager()
{
    return m_playerManager.get();
}

IEnemyManager* GameWorld::getEnemyManager()
{
    return m_enemyManager.get();
}

IItemManager* GameWorld::getItemManager()
{
    return m_itemManager.get();
}

IHUDManager* GameWorld::getHUDManager()
{
    return m_hudManager.get();
}

ISaveManager* GameWorld::getSaveManager()
{
    return m_saveManager.get();
}
