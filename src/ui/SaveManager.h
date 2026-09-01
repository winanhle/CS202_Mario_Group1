#pragma once

#include "../interfaces/ISaveManager.h"
#include <string>
#include <vector>

/**
 * @class SaveManager
 * @brief Saves and loads game state (score, lives, position) and high scores to disk.
 *
 * Developer: Nguyen Phuc
 *
 * Saves and loads game state (score, lives, position) to a file.
 * Uses a simple text-based format for easy debugging.
 */
class SaveManager : public ISaveManager
{
public:
    SaveManager();
    ~SaveManager() override = default;

    void initialize() override;
    bool saveGame(const GameMemento& memento) override;
    std::optional<GameMemento> loadGame() override;
    bool hasSaveFile() const override;
    void deleteSave() override;

    // High score management
    std::vector<ScoreEntry> loadHighScores() const override;
    void saveHighScores(const std::vector<ScoreEntry>& entries) override;
    bool isHighScore(int score) const override;
    void addHighScore(const std::string& initials, int score) override;

    // Stage progression management
    int getMaxUnlockedStage() const override;
    void unlockStage(int stageNumber) override;

private:
    std::string getSaveFilePath() const;
    std::string getHighScoresFilePath() const;
    std::string getProgressionFilePath() const;
    void loadProgression();

    static constexpr const char* SAVE_FILE = "mario_save.dat";
    static constexpr const char* SCORES_FILE = "scores.dat";
    static constexpr const char* PROGRESSION_FILE = "progression.dat";

    bool m_hasSave = false;
    int m_maxUnlockedStage = 1;
};