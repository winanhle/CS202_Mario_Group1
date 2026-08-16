#pragma once

#include <SFML/Graphics/Rect.hpp>
#include "IMapManager.h"
class IEnemyManager;

class ISettingsManager;

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @enum FormType
 * @brief Represents the current power-up form of the player.
 *        Used by MapManager::onHitFromBelow() to decide which item to spawn.
 */
enum class FormType {
    Normal, // Small Mario / Luigi
    Super,  // Big (Mushroom)
    Fire    // Fire (FireFlower)
};

/**
 * @interface IPlayerManager
 * @brief Interface for player management module
 * 
 * Implemented by: Le Tran
 * Responsible for: Mario character, player input, player state
 * 
 * Extension Point:
 * - Le Tran should create PlayerManager implementing this interface
 * - No other modules should directly depend on PlayerManager
 * - Communication happens through this interface via GameWorld
 */
class IPlayerManager
{
public:
    virtual ~IPlayerManager() = default;

    /**
     * @brief Initialize the player manager
     * @param settings Optional settings manager; when provided, the player
     *        builds its key bindings from the saved settings instead of
     *        hardcoded defaults
     */
    virtual void initialize(ISettingsManager* settings = nullptr) = 0;

    /**
     * @brief Update player logic
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render player to screen
     * @param window SFML window
     */
    virtual void render(sf::RenderWindow& window) const = 0;

    /**
     * @brief Handle input events
     * @param event SFML event
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Check if player is alive
     * @return True if player is alive
     */
    virtual bool isAlive() const = 0;

    /**
     * @brief Get player score
     * @return Current score
     */
    virtual int getScore() const = 0;

    /**
     * @brief Get remaining lives.
     * @note Ở chế độ 2P, lives được quản lý tập trung bởi GameWorld (shared lives pool).
     *       getLives() trên mỗi player instance không còn có ý nghĩa — dùng GameWorld::getSharedLives() thay thế.
     *       Method được giữ lại để backward compatible.
     */
    virtual int getLives() const { return 0; }

    /**
     * @brief Get player position for queries
     * @return X coordinate of player
     */
    virtual float getPositionX() const = 0;

    /**
     * @brief Get player position for queries
     * @return Y coordinate of player
     */
    virtual float getPositionY() const = 0;

    /**
     * @brief Restore a previously saved player state (score, lives, position)
     * @param score Saved score
     * @param lives Saved lives
     * @param posX Saved X position
     * @param posY Saved Y position
     */
    virtual void restoreState(int score, int lives, float posX, float posY) = 0;

    // ─── API CHO ENEMY/ITEM MANAGER ───
    virtual sf::FloatRect getHitbox() const = 0;

    // ─── BEHAVIOR ───
    virtual void setMapManager(IMapManager* map) = 0;
    virtual void takeDamage() = 0;
    virtual void bounce() = 0;
    virtual void collectCoin(int amount) = 0;
    virtual void collectPowerUp(int type) = 0;

    /**
     * @brief Instantly kills the player regardless of current form.
     *        Used by DEATH_ZONE tile contact.
     */
    virtual void die() = 0;

    /**
     * @brief Returns the player's current power-up form.
     *        Used by MapManager to decide Mushroom vs FireFlower spawn.
     */
    virtual FormType getFormType() const = 0;

    // ─── FIREBALL ───
    /**
     * @brief Gán EnemyManager làm mục tiêu cho cầu lửa của player.
     * Được GameWorld inject sau initialize().
     */
    virtual void setFireballEnemyTarget(IEnemyManager* enemies) = 0;

    /**
     * @brief Hồi sinh player tại vị trí spawn, cấp i-frames.
     * Được GameWorld gọi khi shared lives pool vẫn còn > 0 sau khi cả 2 player chết.
     */
    virtual void respawn() = 0;
};
