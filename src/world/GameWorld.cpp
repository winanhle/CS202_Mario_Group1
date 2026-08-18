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
    // Khám phá tất cả stageN.tmx trong assets/map → thứ tự chơi.
    m_levelManager.discoverLevels("assets/map/");

    if (m_mapManager)
        m_mapManager->initialize();

    if (m_playerManager)
        m_playerManager->initialize(m_settings.get());

    if (m_playerManager2)
        m_playerManager2->initialize(m_settings.get());

    if (m_enemyManager)
        m_enemyManager->initialize();

    if (m_itemManager)
        m_itemManager->initialize();

    if (m_hudManager)
        m_hudManager->initialize();

    if (m_saveManager)
        m_saveManager->initialize();

    injectDependencies();

    // Tải stage hiện tại: map + enemy + item + player spawn + camera.
    loadCurrentLevel();

    m_isInitialized = true;
}

// ==================== LEVEL PROGRESSION ====================

void GameWorld::loadCurrentLevel()
{
    if (!m_mapManager)
        return;

    const std::string levelPath = m_levelManager.getCurrentLevelPath();
    if (levelPath.empty())
        return;

    m_mapManager->loadMap(levelPath);

    const MapObjectData& mapData = m_mapManager->getMapObjectData();

    if (m_enemyManager)
        m_enemyManager->spawnFromMapData(mapData.enemySpawns);

    if (m_itemManager)
    {
        for (const auto& item : mapData.itemSpawns)
            m_itemManager->spawnStar(item.x, item.y);
    }

    if (mapData.playerSpawn.found)
    {
        const float sx = mapData.playerSpawn.x;
        const float sy = mapData.playerSpawn.y;
        if (m_playerManager)
        {
            m_playerManager->setSpawnPoint(sx, sy);
            m_playerManager->respawn();
        }
        if (m_playerManager2)
        {
            m_playerManager2->setSpawnPoint(sx, sy);
            m_playerManager2->respawn();
        }
    }

    if (m_cameraManager)
        m_cameraManager->initialize(m_mapManager->getMapPixelSize());
}

bool GameWorld::hitboxTouchesFlagpole(const sf::FloatRect& box) const
{
    if (!m_mapManager)
        return false;

    const int tileSize = m_mapManager->getTileSize();
    const int x0 = static_cast<int>(box.position.x) / tileSize;
    const int x1 = static_cast<int>(box.position.x + box.size.x - 1.f) / tileSize;
    const int y0 = static_cast<int>(box.position.y) / tileSize;
    const int y1 = static_cast<int>(box.position.y + box.size.y - 1.f) / tileSize;

    for (int gy = y0; gy <= y1; ++gy)
    {
        for (int gx = x0; gx <= x1; ++gx)
        {
            const float cx = static_cast<float>(gx) * tileSize + tileSize / 2.f;
            const float cy = static_cast<float>(gy) * tileSize + tileSize / 2.f;
            if (m_mapManager->getTileType(cx, cy) == TileType::FLAGPOLE)
                return true;
        }
    }
    return false;
}

void GameWorld::checkFlagpoleCollision()
{
    bool reached = false;

    if (m_playerManager && m_playerManager->isAlive())
        reached = hitboxTouchesFlagpole(m_playerManager->getHitbox());

    if (!reached && m_playerManager2 && m_playerManager2->isAlive())
        reached = hitboxTouchesFlagpole(m_playerManager2->getHitbox());

    if (!reached)
        return;

    // Stage cuối → quay vòng về stage 1 để game tiếp tục chơi được.
    if (!m_levelManager.advanceToNextLevel())
        m_levelManager.reset();

    loadCurrentLevel();
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

    // 1.5. Chạm FLAGPOLE → sang stage kế tiếp (map mới tải lại từ đầu)
    checkFlagpoleCollision();

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

        if (m_itemManager)
            m_hudManager->updateItemCount(m_itemManager->getItemCount());
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

    // Còn lives → reload nguyên cả stage từ đầu (reset map, enemy, item, player)
    loadCurrentLevel();
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
    if (m_hudManager && m_hudManager->isTimeUp())
        return true;
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
        m_enemyManager->setPlayerManager2(m_playerManager2.get());
    }

    // FireballManager ← EnemyManager (để cầu lửa có thể tiêu diệt enemy)
    if (m_playerManager)
        m_playerManager->setFireballEnemyTarget(m_enemyManager.get());

    if (m_playerManager2)
        m_playerManager2->setFireballEnemyTarget(m_enemyManager.get());

    // ItemManager ← PlayerManager(s)
    if (m_itemManager)
    {
        m_itemManager->setPlayerManager(m_playerManager.get());
        m_itemManager->setMapManager(m_mapManager.get());
        m_itemManager->setPlayerManager2(m_playerManager2.get());
    }

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
    if (m_playerManager) {
        m_playerManager->setPlayerIndex(1);
        m_playerManager->setTwoPlayerMode(m_playerManager2 != nullptr);
    }
}

void GameWorld::setPlayerManager2(std::shared_ptr<IPlayerManager> playerManager2)
{
    m_playerManager2 = playerManager2;
    if (m_playerManager2) {
        m_playerManager2->setPlayerIndex(2);
        m_playerManager2->setTwoPlayerMode(true);
    }
    if (m_playerManager) {
        m_playerManager->setTwoPlayerMode(m_playerManager2 != nullptr);
    }
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

void GameWorld::setSettings(std::shared_ptr<ISettingsManager> settings)
{
    m_settings = std::move(settings);
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
