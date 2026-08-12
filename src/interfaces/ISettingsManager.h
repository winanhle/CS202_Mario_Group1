#pragma once

#include <SFML/Window/Keyboard.hpp>

/**
 * @enum GameAction
 * @brief Logical game actions that can be rebound by the player.
 *
 * Kept independent from any concrete input module (e.g. Le Tran's
 * KeyBinding struct) so the settings system does not depend on the
 * player module. The player module can map these actions to its own
 * bindings later.
 */
enum class GameAction
{
    Jump,
    MoveLeft,
    MoveRight,
    Count // number of actions, must stay last
};

/**
 * @interface ISettingsManager
 * @brief Interface for game settings (volume, key bindings) and persistence.
 *
 * Implemented by: Nguyen Phuc
 * Responsible for: storing user preferences and persisting them to disk.
 *
 * Extension Point:
 * - SettingsManager implements this interface
 * - States receive it via dependency injection (constructor)
 * - No other module should depend on the concrete SettingsManager
 */
class ISettingsManager
{
public:
    virtual ~ISettingsManager() = default;

    /**
     * @brief Load settings from disk (or apply defaults if none exist)
     */
    virtual void initialize() = 0;

    /**
     * @brief Get the master volume (0..100)
     */
    virtual float getVolume() const = 0;

    /**
     * @brief Set the master volume (clamped to 0..100)
     */
    virtual void setVolume(float volume) = 0;

    /**
     * @brief Get the key bound to a game action
     */
    virtual sf::Keyboard::Key getKey(GameAction action) const = 0;

    /**
     * @brief Bind a key to a game action
     */
    virtual void setKey(GameAction action, sf::Keyboard::Key key) = 0;

    /**
     * @brief Persist settings to disk
     */
    virtual void save() = 0;

    /**
     * @brief Reset settings to default values
     */
    virtual void resetToDefault() = 0;
};