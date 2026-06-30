#include "GameWorld.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "../interfaces/IMapManager.h"
#include "../interfaces/IPlayerManager.h"
#include "../interfaces/IEnemyManager.h"
#include "../interfaces/IItemManager.h"
#include "../interfaces/IHUDManager.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/ICameraManager.h"

GameWorld::GameWorld()
    : m_isInitialized(false)
{
}

void GameWorld::initialize()
{
    if (m_mapManager)
    {
        m_mapManager->initialize();
    }

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

    if (m_cameraManager && m_mapManager)
    {
        m_cameraManager->initialize(m_mapManager->getMapPixelSize());
    }

    injectDependencies();
    m_isInitialized = true;
}

void GameWorld::update(float deltaTime)
{
    if (!m_isInitialized)
        return;

    if (m_mapManager)
    {
        m_mapManager->update(deltaTime);
    }

    // 1. Player di chuyển + tile collision → vị trí FINAL
    if (m_playerManager)
    {
        m_playerManager->update(deltaTime);
    }

    // 2. Enemy check va chạm với player position MỚI
    if (m_enemyManager)
    {
        m_enemyManager->update(deltaTime);
    }

    // 3. Item check va chạm với player position MỚI
    if (m_itemManager)
    {
        m_itemManager->update(deltaTime);
    }

    if (m_hudManager)
    {
        m_hudManager->update(deltaTime);
    }

    // 4. Camera bám theo player position (đã final)
    if (m_cameraManager)
    {
        m_cameraManager->update(deltaTime);
    }
}

void GameWorld::render(sf::RenderWindow& window) const
{
    if (!m_isInitialized)
        return;

    // Áp dụng camera view trước khi render cảnh game
    if (m_cameraManager)
    {
        window.setView(m_cameraManager->getView());
    }

    // TODO: Render game world (background, tiles, etc.)
    // This will be extended by the Level/World module
    if (m_mapManager)
    {
        m_mapManager->render(window);
    }

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

    // HUD dùng default view (không bị ảnh hưởng camera)
    window.setView(window.getDefaultView());

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

// ==================== INJECT DEPENDENCIES ====================

void GameWorld::injectDependencies() {
    // PlayerManager ← MapManager (cho tile collision)
    if (m_playerManager)
        m_playerManager->setMapManager(m_mapManager.get());

    // EnemyManager ← PlayerManager
    if (m_enemyManager) {
        m_enemyManager->setPlayerManager(m_playerManager.get());
    }

    // ItemManager ← PlayerManager
    if (m_itemManager) {
        m_itemManager->setPlayerManager(m_playerManager.get());
    }

    // CameraManager ← PlayerManager (read-only)
    if (m_cameraManager && m_playerManager)
    {
        m_cameraManager->setFollowTarget(m_playerManager.get());
    }
}

// ==================== EXTENSION POINTS ====================

void GameWorld::setMapManager(std::shared_ptr<IMapManager> mapManager)
{
    m_mapManager = mapManager;
}

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

IMapManager* GameWorld::getMapManager()
{
    return m_mapManager.get();
}

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

void GameWorld::setCameraManager(std::shared_ptr<ICameraManager> cameraManager)
{
    m_cameraManager = cameraManager;
}

ICameraManager* GameWorld::getCameraManager()
{
    return m_cameraManager.get();
}
