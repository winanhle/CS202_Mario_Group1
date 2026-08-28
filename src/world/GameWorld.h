#pragma once

#include <memory>
#include <SFML/Graphics/Rect.hpp>
#include "../core/LevelManager.h"

// Forward declarations for all modules
class IMapManager;
class IPlayerManager;
class IEnemyManager;
class IItemManager;
class IHUDManager;
class ISaveManager;
class ICameraManager;
class ISettingsManager;

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @class GameWorld
 * @brief Central coordinator for all game modules
 *
 * GameWorld acts as the mediator/facade that coordinates all major systems.
 * It provides extension points for each module without requiring them to know about each other.
 *
 * Ownership:
 * - Owns all manager instances (Player 1 & 2, Enemy, Item, HUD, SaveManager)
 * - Owns shared lives pool (m_sharedLives) for both 1P and 2P modes
 * - Provides controlled access to each system
 */
class GameWorld
{
public:
    GameWorld();
    ~GameWorld() = default;

    // Delete copy operations - GameWorld is unique
    GameWorld(const GameWorld&) = delete;
    GameWorld& operator=(const GameWorld&) = delete;

    /**
     * @brief Initialize the game world
     * Called once at startup after all managers are set
     */
    void initialize();

    /**
     * @brief Update all game systems
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime);

    /**
     * @brief Render all game systems
     * @param window The SFML window to render to
     */
    void render(sf::RenderWindow& window) const;

    /**
     * @brief Handle input events
     * @param event The SFML event
     */
    void handleInput(const sf::Event& event);

    /**
     * @brief Returns true when shared lives pool is exhausted.
     */
    bool isGameOver() const;

    /**
     * @brief Returns true when the final level is cleared (flagpole reached).
     */
    bool isGameWon() const;

    /**
     * @brief Returns true when a non-final stage is cleared (ready for intermission).
     */
    bool isStageClear() const;

    /**
     * @brief Advances to the next stage and reloads map / entities.
     */
    void advanceStage();

    /**
     * @brief Set specific stage number and reloads level (1-based)
     */
    void setStage(int stageNumber);

    int getCurrentStageNumber() const;
    int getNextStageNumber() const;

    /**
     * @brief Returns combined score of all active players.
     */
    int getTotalScore() const;

    /**
     * @brief Returns combined coin count of all active players.
     */
    int getTotalCoins() const;

    /**
     * @brief Returns current shared lives count.
     */
    int getSharedLives() const;
    void setSharedLives(int lives);

    /**
     * @brief Deletes any saved progress on game end / completion.
     */
    void deleteSaveData();

    // ─── sau initialize, inject dependency ───
    void injectDependencies();

    // ==================== EXTENSION POINTS ====================

    void setMapManager(std::shared_ptr<IMapManager> mapManager);
    IMapManager* getMapManager();

    /** Player 1 (hoặc player duy nhất ở chế độ 1P) */
    void setPlayerManager(std::shared_ptr<IPlayerManager> playerManager);

    /** Player 2 – chỉ set ở chế độ 2P */
    void setPlayerManager2(std::shared_ptr<IPlayerManager> playerManager2);

    void setEnemyManager(std::shared_ptr<IEnemyManager> enemyManager);
    void setItemManager(std::shared_ptr<IItemManager> itemManager);
    void setHUDManager(std::shared_ptr<IHUDManager> hudManager);
    void setSaveManager(std::shared_ptr<ISaveManager> saveManager);
    void setCameraManager(std::shared_ptr<ICameraManager> cameraManager);

    /**
     * @brief Set the shared settings manager (injected from Game)
     * Used to pass key bindings down to the player module.
     */
    void setSettings(std::shared_ptr<ISettingsManager> settings);

    // ==================== ACCESSORS ====================

    IPlayerManager* getPlayerManager();
    IPlayerManager* getPlayerManager2();
    IEnemyManager*  getEnemyManager();
    IItemManager*   getItemManager();
    IHUDManager*    getHUDManager();
    ISaveManager*   getSaveManager();
    ICameraManager* getCameraManager();

private:
    // --- Module manager instances ---
    std::shared_ptr<IMapManager>    m_mapManager;
    std::shared_ptr<IPlayerManager> m_playerManager;
    std::shared_ptr<IPlayerManager> m_playerManager2; // null trong 1P mode
    std::shared_ptr<IEnemyManager>  m_enemyManager;
    std::shared_ptr<IItemManager>   m_itemManager;
    std::shared_ptr<IHUDManager>    m_hudManager;
    std::shared_ptr<ISaveManager>   m_saveManager;
    std::shared_ptr<ICameraManager> m_cameraManager;
    std::shared_ptr<ISettingsManager> m_settings;

    // --- Shared lives pool (1P & 2P) ---
    static constexpr int INITIAL_LIVES = 3;
    int  m_sharedLives  = INITIAL_LIVES;
    bool m_isGameOver   = false;
    bool m_isGameWon    = false;
    bool m_isStageClear = false;
    bool m_isFlagpoleSequenceActive = false;
    bool m_isTimerTallyActive = false;
    int  m_timerPopupCounter = 0;
    bool m_isInitialized = false;
    int  m_lastTotalScore = -1;

    /**
     * @brief Kiểm tra điều kiện "round death" và xử lý respawn / game over.
     *
     * 1P:  player1 chết → trừ 1 shared live → respawn nếu còn live.
     * 2P:  cả 2 player đều chết → trừ 1 shared live → respawn cả 2 nếu còn live.
     */
    void checkAndHandleDeath();

    // ─── Level progression ──────────────────────────────────────────────────
    LevelManager m_levelManager;

    /**
     * @brief Tải lại stage hiện tại từ đầu: map + enemy + item + player spawn,
     *        đồng thời reset camera theo kích thước map mới.
     */
    void loadCurrentLevel();

    /**
     * @brief Kiểm tra player chạm cột FLAGPOLE → advance lên stage tiếp theo.
     *        Nếu đã ở stage cuối thì quay vòng lại stage 1.
     */
    void checkFlagpoleCollision();

    /**
     * @brief Kiểm tra bất kỳ ô FLAGPOLE nào nằm trong hitbox.
     */
    bool hitboxTouchesFlagpole(const sf::FloatRect& box) const;

    /**
     * @brief Tìm tọa độ X của cột cờ mà hitbox đang chạm.
     */
    float getFlagpoleTileX(const sf::FloatRect& box) const;
};
