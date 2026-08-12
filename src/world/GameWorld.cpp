#include "GameWorld.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "../interfaces/IMapManager.h"
#include "../interfaces/IPlayerManager.h"
#include "../interfaces/IEnemyManager.h"
#include "../interfaces/IItemManager.h"
#include "../interfaces/IHUDManager.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/ICameraManager.h"
#include "../entities/map/MapManager.h"

GameWorld::GameWorld()
    : m_sharedLives(INITIAL_LIVES)
    , m_isGameOver(false)
    , m_isInitialized(false)
{
}

// ==================== INITIALIZE ====================

void GameWorld::initialize()
{
    if (m_mapManager)
        m_mapManager->initialize();

    if (m_playerManager)
        m_playerManager->initialize();

    if (m_playerManager2)
        m_playerManager2->initialize();

    if (m_enemyManager)
        m_enemyManager->initialize();

    if (m_itemManager)
        m_itemManager->initialize();

    if (m_hudManager)
        m_hudManager->initialize();

    if (m_saveManager)
        m_saveManager->initialize();

    if (m_cameraManager && m_mapManager)
        m_cameraManager->initialize(m_mapManager->getMapPixelSize());

    injectDependencies();
    m_isInitialized = true;
}

// ==================== UPDATE ====================

void GameWorld::update(float deltaTime)
{
    if (!m_isInitialized || m_isGameOver)
        return;

    if (m_mapManager)
        m_mapManager->update(deltaTime);

    // 1. Player(s) di chuyển + tile collision → vị trí FINAL
    if (m_playerManager)
        m_playerManager->update(deltaTime);

    if (m_playerManager2)
        m_playerManager2->update(deltaTime);

    // 2. Enemy check va chạm với player position MỚI
    if (m_enemyManager)
        m_enemyManager->update(deltaTime);

    // 3. Item check va chạm với player position MỚI
    if (m_itemManager)
        m_itemManager->update(deltaTime);

    // 4. Kiểm tra & xử lý death / respawn / game over
    checkAndHandleDeath();

    // 5. HUD update
    if (m_hudManager)
    {
        // Score: tổng của cả 2 player
        m_hudManager->updateScore(getTotalScore());
        // Lives: shared pool
        m_hudManager->updateLives(m_sharedLives);

        if (m_enemyManager)
            m_hudManager->updateEnemyCount(m_enemyManager->getEnemyCount());

        m_hudManager->update(deltaTime);
    }

    // 6. Camera bám theo player (đã final)
    if (m_cameraManager)
        m_cameraManager->update(deltaTime);
}

// ==================== DEATH / RESPAWN LOGIC ====================

void GameWorld::checkAndHandleDeath()
{
    bool p1Dead = m_playerManager  && !m_playerManager->isAlive();
    bool p2Dead = !m_playerManager2 || !m_playerManager2->isAlive(); // true nếu không có P2

    bool roundOver = false;

    if (m_playerManager2)
    {
        // 2P mode: round over khi CẢ HAI chết
        roundOver = p1Dead && p2Dead;
    }
    else
    {
        // 1P mode: round over khi P1 chết
        roundOver = p1Dead;
    }

    if (!roundOver) return;

    // Trừ 1 shared live
    --m_sharedLives;

    if (m_sharedLives <= 0)
    {
        m_sharedLives = 0;
        m_isGameOver  = true;
        return;
    }

    // Còn lives → respawn cả 2 player
    if (m_playerManager)
        m_playerManager->respawn();

    if (m_playerManager2)
        m_playerManager2->respawn();
}

// ==================== RENDER ====================

void GameWorld::render(sf::RenderWindow& window) const
{
    if (!m_isInitialized) return;

    // Áp dụng camera view trước khi render cảnh game
    if (m_cameraManager)
        window.setView(m_cameraManager->getView());

    if (m_mapManager)
        m_mapManager->render(window);

    if (m_enemyManager)
        m_enemyManager->render(window);

    if (m_itemManager)
        m_itemManager->render(window);

    if (m_playerManager)
        m_playerManager->render(window);

    if (m_playerManager2)
        m_playerManager2->render(window);

    // HUD dùng default view (không bị ảnh hưởng camera)
    window.setView(window.getDefaultView());

    if (m_hudManager)
        m_hudManager->render(window);
}

// ==================== HANDLE INPUT ====================

void GameWorld::handleInput(const sf::Event& event)
{
    if (!m_isInitialized) return;

    if (m_playerManager)
        m_playerManager->handleInput(event);

    if (m_playerManager2)
        m_playerManager2->handleInput(event);

    if (m_hudManager)
        m_hudManager->handleInput(event);
}

// ==================== GAME OVER ====================

bool GameWorld::isGameOver() const
{
    return m_isGameOver;
}

int GameWorld::getTotalScore() const
{
    int total = 0;
    if (m_playerManager)  total += m_playerManager->getScore();
    if (m_playerManager2) total += m_playerManager2->getScore();
    return total;
}

int GameWorld::getSharedLives() const
{
    return m_sharedLives;
}

// ==================== INJECT DEPENDENCIES ====================

void GameWorld::injectDependencies()
{
    // PlayerManager ← MapManager (cho tile collision)
    if (m_playerManager)
        m_playerManager->setMapManager(m_mapManager.get());

    if (m_playerManager2)
        m_playerManager2->setMapManager(m_mapManager.get());

    // EnemyManager ← PlayerManager (P1 làm primary target)
    if (m_enemyManager) {
        m_enemyManager->setPlayerManager(m_playerManager.get());
        m_enemyManager->setMapManager(m_mapManager.get());
    }

    // FireballManager ← EnemyManager (để cầu lửa có thể tiêu diệt enemy)
    if (m_playerManager)
        m_playerManager->setFireballEnemyTarget(m_enemyManager.get());

    if (m_playerManager2)
        m_playerManager2->setFireballEnemyTarget(m_enemyManager.get());

    // ItemManager ← PlayerManager (P1 làm primary target)
    if (m_itemManager)
        m_itemManager->setPlayerManager(m_playerManager.get());

    // MapManager ← ItemManager (để map có thể spawn item khi block bị đập)
    if (m_mapManager)
        m_mapManager->setItemManager(m_itemManager.get());

    // CameraManager ← PlayerManager(s)
    if (m_cameraManager)
    {
        if (m_playerManager2)
            m_cameraManager->setFollowTargets(m_playerManager.get(), m_playerManager2.get());
        else
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

void GameWorld::setPlayerManager2(std::shared_ptr<IPlayerManager> playerManager2)
{
    m_playerManager2 = playerManager2;
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

void GameWorld::setCameraManager(std::shared_ptr<ICameraManager> cameraManager)
{
    m_cameraManager = cameraManager;
}

// ==================== ACCESSORS ====================

IMapManager*    GameWorld::getMapManager()    { return m_mapManager.get(); }
IPlayerManager* GameWorld::getPlayerManager() { return m_playerManager.get(); }
IPlayerManager* GameWorld::getPlayerManager2(){ return m_playerManager2.get(); }
IEnemyManager*  GameWorld::getEnemyManager()  { return m_enemyManager.get(); }
IItemManager*   GameWorld::getItemManager()   { return m_itemManager.get(); }
IHUDManager*    GameWorld::getHUDManager()    { return m_hudManager.get(); }
ISaveManager*   GameWorld::getSaveManager()   { return m_saveManager.get(); }
ICameraManager* GameWorld::getCameraManager() { return m_cameraManager.get(); }
