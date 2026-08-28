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
    bool saveGame() override;
    bool loadGame() override;
    bool hasSaveFile() const override;
    void deleteSave() override;

    // Additional helpers for setting/reading saved data
    void setSaveData(int score, int lives, int level, int coins = 0) override;
    int getSavedScore() const override;
    int getSavedLives() const override;
    int getSavedLevel() const override;
    int getSavedCoins() const override;

    void setGameConfig(int p1Char, int p2Char, int mode) override;
    int getSavedP1Char() const override;
    int getSavedP2Char() const override;
    int getSavedMode() const override;

    // High score management
    std::vector<ScoreEntry> loadHighScores() const override;
    void saveHighScores(const std::vector<ScoreEntry>& entries) override;
    bool isHighScore(int score) const override;
    void addHighScore(const std::string& initials, int score) override;

private:
    std::string getSaveFilePath() const;
    std::string getHighScoresFilePath() const;

    static constexpr const char* SAVE_FILE = "mario_save.dat";
    static constexpr const char* SCORES_FILE = "scores.dat";

    bool m_hasSave = false;

    // Saved game state
    int m_savedScore = 0;
    int m_savedLives = 3;
    int m_savedLevel = 1;
    int m_savedCoins = 0;

    int m_savedP1Char = 0;
    int m_savedP2Char = 1;
    int m_savedMode = 0;
};