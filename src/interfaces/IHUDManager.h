#pragma once

#include <string>
#include <functional>
#include "../core/GameConfig.h"

namespace sf {
class RenderWindow;
class Event;
class View;
}

/**
 * @interface IHUDManager
 * @brief Interface for HUD/UI management module
 * 
 * Implemented by: Nguyen Phuc
 * Responsible for: UI rendering, HUD display, score display, menus
 * 
 * Extension Point:
 * - Nguyen Phuc should create HUDManager implementing this interface
 * - No other modules should directly depend on HUDManager
 * - Communication happens through this interface via GameWorld
 */
class IHUDManager
{
public:
    virtual ~IHUDManager() = default;

    /**
     * @brief Initialize the HUD manager
     */
    virtual void initialize() = 0;

    /**
     * @brief Update HUD logic (e.g., animations, state changes)
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Render HUD to screen
     * @param window SFML window
     */
    virtual void render(sf::RenderWindow& window) const = 0;

    /**
     * @brief Handle input events (e.g., menu selections)
     * @param event SFML event
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Update HUD with player score
     * @param score The score to display
     */
    virtual void updateScore(int score) = 0;

    /**
     * @brief Update HUD with player lives
     * @param lives Number of lives to display
     */
    virtual void updateLives(int lives) = 0;

    /**
     * @brief Update HUD with item count
     * @param count Number of items in the world
     */
    virtual void updateItemCount(int count) = 0;

    /**
     * @brief Check whether the countdown timer has reached zero
     * @return True when time is up (game over condition)
     */
    virtual bool isTimeUp() const = 0;

    /**
     * @brief Update HUD with current world/level number
     * @param level Current level number (1-based)
     */
    virtual void updateWorld(int level) = 0;

    /**
     * @brief Reset the countdown timer back to its starting value.
     *        Called when a new level is loaded.
     */
    virtual void resetTimer() = 0;

    /**
     * @brief Set the active character type for hero icon display
     * @param characterType CharacterType enum
     */
    virtual void setCharacter(CharacterType characterType) = 0;

    /**
     * @brief Display a transient notification banner on the screen
     * @param message Text to display
     * @param duration Duration in seconds to show the toast
     */
    virtual void showToast(const std::string& message, float duration = 2.0f) = 0;

    /**
     * @brief Spawn a floating score popup in world space
     * @param points Score amount
     * @param worldX World X position
     * @param worldY World Y position
     */
    virtual void spawnScorePopup(int points, float worldX, float worldY, bool isOneUp = false) = 0;

    /**
     * @brief Render floating score popups.
     * @param window SFML RenderWindow
     * @param cameraView Optional pointer to the camera view for crisp world->screen space rendering
     */
    virtual void renderPopups(sf::RenderWindow& window, const sf::View* cameraView = nullptr) const = 0;

    /**
     * @brief Start rapid drain of remaining time to convert into score
     * @param onScoreTick Callback invoked with bonus points awarded per second drained
     * @param onComplete Callback invoked when countdown tally finishes
     * @param worldX World X position where accumulating score popup appears
     * @param worldY World Y position where accumulating score popup appears
     */
    virtual void startTimerBonus(std::function<void(int bonus)> onScoreTick, std::function<void()> onComplete = nullptr, float worldX = 0.f, float worldY = 0.f) = 0;

    /**
     * @brief Check if rapid timer bonus countdown is currently running
     */
    virtual bool isTimerBonusActive() const = 0;

    /**
     * @brief Get current remaining time in seconds
     */
    virtual float getTimeLeft() const = 0;

    /**
     * @brief Display an achievement unlock banner on the HUD
     * @param title Achievement title to display
     */
    virtual void showAchievementToast(const std::string& title) = 0;
};

