#pragma once

#include "../core/GameMemento.h"
#include <string>
#include <vector>
#include <optional>

/**
 * @struct ScoreEntry
 * @brief Represents a single high score leaderboard entry
 */
struct ScoreEntry
{
    std::string initials = "AAAAA";
    int score = 0;
};

/**
 * @interface ISaveManager
 * @brief Interface for save/load management module acting as the Caretaker in the Memento pattern.
 * 
 * Implemented by: Nguyen Phuc
 * Responsible for: Game state serialization/deserialization, game state persistence, high scores
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
     * @brief Save the given game state snapshot (Memento) to persistent storage
     * @param memento Snapshot containing all game state data
     * @return True if save was successful
     */
    virtual bool saveGame(const GameMemento& memento) = 0;

    /**
     * @brief Load the game state snapshot (Memento) from persistent storage
     * @return GameMemento if load was successful, std::nullopt otherwise
     */
    virtual std::optional<GameMemento> loadGame() = 0;

    /**
     * @brief Check if a save file exists
     * @return True if save file exists
     */
    virtual bool hasSaveFile() const = 0;

    /**
     * @brief Delete the save file (e.g. on game over or game completion)
     */
    virtual void deleteSave() = 0;

    // --- HIGH SCORES ---
    virtual std::vector<ScoreEntry> loadHighScores() const = 0;
    virtual void saveHighScores(const std::vector<ScoreEntry>& entries) = 0;
    virtual bool isHighScore(int score) const = 0;
    virtual void addHighScore(const std::string& initials, int score) = 0;
};

