#pragma once

/**
 * @interface ISaveManager
 * @brief Interface for save/load management module
 * 
 * Implemented by: Nguyen Phuc
 * Responsible for: Game state saving, game state loading, persistence
 * 
 * Extension Point:
 * - Nguyen Phuc should create SaveManager implementing this interface
 * - No other modules should directly depend on SaveManager
 * - Communication happens through this interface via GameWorld
 */
class ISaveManager
{
public:
    virtual ~ISaveManager() = default;

    /**
     * @brief Initialize the save manager
     */
    virtual void initialize() = 0;

    /**
     * @brief Save the current game state
     * @return True if save was successful
     */
    virtual bool saveGame() = 0;

    /**
     * @brief Load a previously saved game state
     * @return True if load was successful
     */
    virtual bool loadGame() = 0;

    /**
     * @brief Check if a save file exists
     * @return True if save file exists
     */
    virtual bool hasSaveFile() const = 0;
};
