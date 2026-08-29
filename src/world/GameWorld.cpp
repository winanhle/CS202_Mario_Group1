#include "GameWorld.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "../interfaces/IMapManager.h"
#include "../interfaces/IPlayerManager.h"
#include "../interfaces/IEnemyManager.h"
#include "../interfaces/IItemManager.h"
#include "../interfaces/IHUDManager.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/ICameraManager.h"
#include "../interfaces/ILiftManager.h"
#include "../interfaces/IFireBarManager.h"
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

    if (m_liftManager)
        m_liftManager->initialize();

    if (m_fireBarManager)
        m_fireBarManager->initialize();

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

    std::string levelPath = m_levelManager.getCurrentLevelPath();
    if (!m_customMapPath.empty())
        levelPath = m_customMapPath;

    if (levelPath.empty())
        return;

    m_mapManager->loadMap(levelPath);

    const MapObjectData& mapData = m_mapManager->getMapObjectData();

    if (m_enemyManager)
        m_enemyManager->spawnFromMapData(mapData.enemySpawns);

    if (m_itemManager)
    {
        for (const auto& item : mapData.itemSpawns)
        {
            if (item.type == "STATIC_COIN")
                m_itemManager->spawnStaticCoin(item.x, item.y);
            else
                m_itemManager->spawnStar(item.x, item.y);
        }
    }

    if (m_liftManager)
        m_liftManager->spawnFromMapData(mapData.liftSpawns);

    if (m_fireBarManager)
        m_fireBarManager->spawnFromMapData(mapData.fireBarSpawns);

    if (mapData.playerSpawn.found)
    {
        const float sx = mapData.playerSpawn.x;
        const float sy = mapData.playerSpawn.y;
        if (m_playerManager)
            m_playerManager->setSpawnPoint(sx, sy);
        if (m_playerManager2)
            m_playerManager2->setSpawnPoint(sx, sy);
    }
    
    // ALWAYS respawn players so their positions are reset (even if map lacks a PlayerSpawn object)
    if (m_playerManager)
        m_playerManager->respawn();
    if (m_playerManager2)
        m_playerManager2->respawn();

    m_isTimerTallyActive = false;

    if (m_cameraManager)
        m_cameraManager->initialize(m_mapManager->getMapPixelSize());

    if (m_hudManager)
    {
        m_hudManager->resetTimer();
        m_hudManager->showToast("WORLD 1-" + std::to_string(getCurrentStageNumber()), 2.5f);
    }
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

float GameWorld::getFlagpoleTileX(const sf::FloatRect& box) const
{
    if (!m_mapManager) return box.position.x;
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
                return static_cast<float>(gx * tileSize);
        }
    }
    return box.position.x;
}

void GameWorld::checkFlagpoleCollision()
{
    if (m_isFlagpoleSequenceActive)
    {
        bool finished = true;
        if (m_playerManager && m_playerManager->isAlive() && !m_playerManager->hasFinishedFlagpole())
            finished = false;
        if (m_playerManager2 && m_playerManager2->isAlive() && !m_playerManager2->hasFinishedFlagpole())
            finished = false;

        if (finished && !m_isTimerTallyActive)
        {
            m_isTimerTallyActive = true;

            auto onDone = [this]() {
                m_isFlagpoleSequenceActive = false;
                m_isTimerTallyActive = false;
                if (m_hudManager)
                {
                    m_hudManager->showToast("STAGE CLEAR!", 2.0f);
                }
                if (m_levelManager.isLastLevel())
                {
                    m_isGameWon = true;
                }
                else
                {
                    m_isStageClear = true;
                }
            };

            if (m_hudManager && m_hudManager->getTimeLeft() > 0.0f)
            {
                float px = 0.f, py = 0.f;
                if (m_playerManager && m_playerManager->isAlive())
                {
                    auto hb = m_playerManager->getHitbox();
                    px = hb.position.x + hb.size.x / 2.f;
                    py = hb.position.y - 6.f;
                }
                else if (m_playerManager2 && m_playerManager2->isAlive())
                {
                    auto hb = m_playerManager2->getHitbox();
                    px = hb.position.x + hb.size.x / 2.f;
                    py = hb.position.y - 6.f;
                }
                else if (m_playerManager)
                {
                    auto hb = m_playerManager->getHitbox();
                    px = hb.position.x + hb.size.x / 2.f;
                    py = hb.position.y - 6.f;
                }

                m_hudManager->startTimerBonus([this](int bonus) {
                    if (m_playerManager && m_playerManager->isAlive())
                        m_playerManager->addScore(bonus);
                    else if (m_playerManager2 && m_playerManager2->isAlive())
                        m_playerManager2->addScore(bonus);
                    else if (m_playerManager)
                        m_playerManager->addScore(bonus);
                }, onDone, px, py);
            }
            else
            {
                onDone();
            }
        }
        return;
    }

    bool touchedP1 = false;
    bool touchedP2 = false;
    float poleX = 0.f;

    if (m_playerManager && m_playerManager->isAlive() && hitboxTouchesFlagpole(m_playerManager->getHitbox()))
    {
        touchedP1 = true;
        poleX = getFlagpoleTileX(m_playerManager->getHitbox());
    }

    if (m_playerManager2 && m_playerManager2->isAlive() && hitboxTouchesFlagpole(m_playerManager2->getHitbox()))
    {
        touchedP2 = true;
        if (!touchedP1)
            poleX = getFlagpoleTileX(m_playerManager2->getHitbox());
    }

    if (!touchedP1 && !touchedP2)
        return;

    m_isFlagpoleSequenceActive = true;

    if (m_mapManager)
    {
        const int tileSize = m_mapManager->getTileSize();
        m_mapManager->triggerFlagSlide(static_cast<int>(poleX) / (tileSize > 0 ? tileSize : 16));
    }

    auto calculateFlagPoints = [this](float pX, const sf::FloatRect& hb) -> int {
        // Standard flagpole dimensions in our map
        const float POLE_BOTTOM = 208.f;
        const float POLE_HEIGHT = 144.f;

        // Use Mario's center Y instead of feet, so Big Mario doesn't get penalized
        float playerCenterY = hb.position.y + (hb.size.y / 2.f);
        float actualPixelsHigh = POLE_BOTTOM - playerCenterY;
        
        float ratio = actualPixelsHigh / POLE_HEIGHT;

        // Classic SMB thresholds mapped as ratios of 153
        if (ratio >= 152.f / 153.f) return 5000;
        if (ratio >= 128.f / 153.f) return 4000;
        if (ratio >= 82.f / 153.f)  return 2000;
        if (ratio >= 58.f / 153.f)  return 800;
        if (ratio >= 18.f / 153.f)  return 400;
        return 100;
    };

    if (touchedP1 && m_playerManager && m_playerManager->isAlive())
    {
        m_playerManager->startFlagpoleSlide(poleX);
        auto hb = m_playerManager->getHitbox();
        const int flagPoints = calculateFlagPoints(poleX, hb);
        m_playerManager->addScore(flagPoints);
        if (m_hudManager)
        {
            m_hudManager->spawnScorePopup(flagPoints, hb.position.x + hb.size.x / 2.f, hb.position.y - 6.f);
        }
    }

    if (touchedP2 && m_playerManager2 && m_playerManager2->isAlive())
    {
        m_playerManager2->startFlagpoleSlide(poleX);
        auto hb = m_playerManager2->getHitbox();
        const int flagPoints = calculateFlagPoints(poleX, hb);
        m_playerManager2->addScore(flagPoints);
        if (m_hudManager)
        {
            m_hudManager->spawnScorePopup(flagPoints, hb.position.x + hb.size.x / 2.f, hb.position.y - 6.f);
        }
    }
}

// ==================== UPDATE ====================

void GameWorld::update(float deltaTime)
{
    if (!m_isInitialized || m_isGameOver || m_isGameWon || m_isStageClear)
        return;

    if (m_mapManager)
        m_mapManager->update(deltaTime);

    // 1. Advance moving platforms (lifts) so frame positions & deltas are computed
    if (m_liftManager)
        m_liftManager->update(deltaTime);

    // 2. Player(s) di chuyển + tile & lift collision → vị trí FINAL & accurate grounded/animation state
    if (m_playerManager)
        m_playerManager->update(deltaTime);

    if (m_playerManager2)
        m_playerManager2->update(deltaTime);

    // 1.5. Chạm FLAGPOLE → sang stage kế tiếp (hoặc chiến thắng)
    checkFlagpoleCollision();

    // 2. Enemy check va chạm với player position MỚI (bỏ qua khi đang trong cutscene cờ)
    if (!m_isFlagpoleSequenceActive && m_enemyManager)
        m_enemyManager->update(deltaTime);

    // 3. Item check va chạm với player position MỚI (bỏ qua khi đang trong cutscene cờ)
    if (!m_isFlagpoleSequenceActive && m_itemManager)
        m_itemManager->update(deltaTime);

    // 3.5. Kiểm tra timer time-up → kill player(s) một lần (bỏ qua khi đang trong cutscene cờ)
    if (!m_isFlagpoleSequenceActive && m_hudManager && m_hudManager->isTimeUp())
    {
        // Guard against die() being called every frame while isTimeUp() stays true.
        // checkAndHandleDeath() below will raise m_isGameOver after the first call.
        if (m_playerManager && m_playerManager->isAlive())
            m_playerManager->die();
        if (m_playerManager2 && m_playerManager2->isAlive())
            m_playerManager2->die();
    }

    // 4. FireBar obstacles rotate and damage player
    if (!m_isFlagpoleSequenceActive && m_fireBarManager)
        m_fireBarManager->update(deltaTime);

    // 5. Kiểm tra & xử lý death / respawn / game over
    checkAndHandleDeath();

    // 5. HUD update
    if (m_hudManager)
    {
        // Check for 100-coin 1-UP rollovers from either player
        if (m_playerManager)
        {
            int oneUps = m_playerManager->consumePendingOneUps();
            for (int i = 0; i < oneUps; ++i)
            {
                ++m_sharedLives;
                float px = m_playerManager->getPositionX();
                float py = m_playerManager->getPositionY();
                m_hudManager->spawnScorePopup(0, px + 8.0f, py - 32.0f, true);
            }
            if (oneUps > 0)
                m_hudManager->showToast("1-UP!", 1.5f);
        }
        if (m_playerManager2)
        {
            int oneUps = m_playerManager2->consumePendingOneUps();
            for (int i = 0; i < oneUps; ++i)
            {
                ++m_sharedLives;
                float px = m_playerManager2->getPositionX();
                float py = m_playerManager2->getPositionY();
                m_hudManager->spawnScorePopup(0, px + 8.0f, py - 32.0f, true);
            }
            if (oneUps > 0)
                m_hudManager->showToast("1-UP!", 1.5f);
        }

        int currentScore = getTotalScore();
        if (!m_isFlagpoleSequenceActive && !m_isTimerTallyActive && m_lastTotalScore >= 0 && currentScore > m_lastTotalScore)
        {
            int diff = currentScore - m_lastTotalScore;
            if (m_playerManager && m_playerManager->isAlive())
            {
                float px = m_playerManager->getPositionX();
                float py = m_playerManager->getPositionY();
                m_hudManager->spawnScorePopup(diff, px + 8.0f, py - 12.0f);
            }
            else if (m_playerManager2 && m_playerManager2->isAlive())
            {
                float px = m_playerManager2->getPositionX();
                float py = m_playerManager2->getPositionY();
                m_hudManager->spawnScorePopup(diff, px + 8.0f, py - 12.0f);
            }
        }
        m_lastTotalScore = currentScore;

        // Score: tổng của cả 2 player
        m_hudManager->updateScore(currentScore);
        // Coins: tổng số coin của player
        m_hudManager->updateItemCount(getTotalCoins());
        // Lives: shared pool
        m_hudManager->updateLives(m_sharedLives);
        
        m_hudManager->updateWorld(m_levelManager.getCurrentLevelNumber());
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

    if (m_liftManager)
        m_liftManager->render(window);

    if (m_fireBarManager)
        m_fireBarManager->render(window);

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

    sf::View camView;
    bool hasCam = (m_cameraManager != nullptr);
    if (hasCam)
        camView = m_cameraManager->getView();

    if (m_hudManager)
    {
        m_hudManager->renderPopups(window, hasCam ? &camView : nullptr);
        m_hudManager->render(window);
    }
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

// ==================== GAME OVER / WIN ====================

bool GameWorld::isGameOver() const
{
    return m_isGameOver;
}

bool GameWorld::isGameWon() const
{
    return m_isGameWon;
}

bool GameWorld::isStageClear() const
{
    return m_isStageClear;
}

void GameWorld::advanceStage()
{
    m_isStageClear = false;
    m_isFlagpoleSequenceActive = false;

    // Clear custom map so that normal level progression resumes
    m_customMapPath = "";

    if (!m_levelManager.advanceToNextLevel())
        m_levelManager.reset();
    loadCurrentLevel();
}

void GameWorld::setStage(int stageNumber)
{
    m_isStageClear = false;
    m_isFlagpoleSequenceActive = false;
    if (stageNumber < 1)
        stageNumber = 1;
    m_levelManager.setCurrentLevel(stageNumber - 1);
    loadCurrentLevel();
}

int GameWorld::getCurrentStageNumber() const
{
    return m_levelManager.getCurrentLevelNumber();
}

int GameWorld::getNextStageNumber() const
{
    return m_levelManager.getCurrentLevelNumber() + 1;
}

int GameWorld::getTotalScore() const
{
    int total = 0;
    if (m_playerManager)  total += m_playerManager->getScore();
    if (m_playerManager2) total += m_playerManager2->getScore();
    return total;
}

int GameWorld::getTotalCoins() const
{
    int total = 0;
    if (m_playerManager)  total += m_playerManager->getCoins();
    if (m_playerManager2) total += m_playerManager2->getCoins();
    return total;
}

int GameWorld::getSharedLives() const
{
    return m_sharedLives;
}

void GameWorld::setSharedLives(int lives)
{
    m_sharedLives = lives;
    if (m_hudManager)
        m_hudManager->updateLives(m_sharedLives);
}

void GameWorld::deleteSaveData()
{
    if (m_saveManager)
        m_saveManager->deleteSave();
}

GameMemento GameWorld::createMemento(const GameConfig& config,
                                     std::optional<int> scoreOverride,
                                     std::optional<int> livesOverride,
                                     std::optional<int> coinsOverride) const
{
    GameMemento memento;
    memento.score = scoreOverride.value_or(getTotalScore());
    memento.lives = livesOverride.value_or(getSharedLives());
    memento.stage = getCurrentStageNumber();
    memento.coins = coinsOverride.value_or(getTotalCoins());
    memento.config = config;
    memento.config.customMapPath = m_customMapPath;
    return memento;
}

void GameWorld::setInitialStage(int stageNumber)
{
    if (stageNumber < 1) stageNumber = 1;
    m_levelManager.setCurrentLevel(stageNumber - 1);
}

void GameWorld::restoreFromMemento(const GameMemento& memento)
{
    m_customMapPath = memento.config.customMapPath;
    if (m_levelManager.getCurrentLevelNumber() != memento.stage)
    {
        setStage(memento.stage);
    }
    setSharedLives(memento.lives);

    if (m_playerManager)
    {
        m_playerManager->restoreState(memento.score, memento.lives,
                                      m_playerManager->getPositionX(),
                                      m_playerManager->getPositionY());
        m_playerManager->setCoins(memento.coins);
    }
    if (m_playerManager2)
    {
        m_playerManager2->restoreState(0, memento.lives,
                                       m_playerManager2->getPositionX(),
                                       m_playerManager2->getPositionY());
        m_playerManager2->setCoins(0);
    }

    m_lastTotalScore = memento.score;

    if (m_hudManager)
    {
        m_hudManager->updateScore(memento.score);
        m_hudManager->updateItemCount(memento.coins);
        m_hudManager->updateLives(memento.lives);
        m_hudManager->updateWorld(memento.stage);
    }
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

    // LiftManager ← PlayerManager(s)
    if (m_liftManager)
    {
        m_liftManager->setPlayerManager(m_playerManager.get());
        m_liftManager->setPlayerManager2(m_playerManager2.get());
    }

    // PlayerManager(s) ← LiftManager (for platform collision & riding)
    if (m_playerManager)
        m_playerManager->setLiftManager(m_liftManager.get());

    if (m_playerManager2)
        m_playerManager2->setLiftManager(m_liftManager.get());

    // FireBarManager ← PlayerManager(s)
    if (m_fireBarManager)
    {
        m_fireBarManager->setPlayerManager(m_playerManager.get());
        m_fireBarManager->setPlayerManager2(m_playerManager2.get());
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

void GameWorld::setLiftManager(std::shared_ptr<ILiftManager> liftManager)
{
    m_liftManager = std::move(liftManager);
}

void GameWorld::setFireBarManager(std::shared_ptr<IFireBarManager> fireBarManager)
{
    m_fireBarManager = std::move(fireBarManager);
}

void GameWorld::setSettings(std::shared_ptr<ISettingsManager> settings)
{
    m_settings = std::move(settings);
}

// ==================== ACCESSORS ====================

IMapManager*     GameWorld::getMapManager()     { return m_mapManager.get(); }
IPlayerManager*  GameWorld::getPlayerManager()  { return m_playerManager.get(); }
IPlayerManager*  GameWorld::getPlayerManager2() { return m_playerManager2.get(); }
IEnemyManager*   GameWorld::getEnemyManager()   { return m_enemyManager.get(); }
IItemManager*    GameWorld::getItemManager()    { return m_itemManager.get(); }
IHUDManager*     GameWorld::getHUDManager()     { return m_hudManager.get(); }
ISaveManager*    GameWorld::getSaveManager()    { return m_saveManager.get(); }
ICameraManager*  GameWorld::getCameraManager()  { return m_cameraManager.get(); }
ILiftManager*    GameWorld::getLiftManager()    { return m_liftManager.get(); }
IFireBarManager* GameWorld::getFireBarManager() { return m_fireBarManager.get(); }
