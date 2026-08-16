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

    /**
     * @brief Delete the save file (e.g. on game over)
     */
    virtual void deleteSave() = 0;

    /**
     * @brief Stage the data to be persisted by the next saveGame() call
     * @param score Player score
     * @param lives Player lives
     * @param posX Player X position
     * @param posY Player Y position
     */
    virtual void setSaveData(int score, int lives, float posX, float posY) = 0;

    /**
     * @brief Get the saved score (valid after loadGame())
     */
    virtual int getSavedScore() const = 0;

    /**
     * @brief Get the saved lives (valid after loadGame())
     */
    virtual int getSavedLives() const = 0;

    /**
     * @brief Get the saved X position (valid after loadGame())
     */
    virtual float getSavedPosX() const = 0;

    /**
     * @brief Get the saved Y position (valid after loadGame())
     */
    virtual float getSavedPosY() const = 0;

    // --- GAME CONFIG ---
    virtual void setGameConfig(int p1Char, int p2Char, int mode) = 0;
    virtual int getSavedP1Char() const = 0;
    virtual int getSavedP2Char() const = 0;
    virtual int getSavedMode() const = 0;
};
