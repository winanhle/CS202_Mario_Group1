#pragma once

#include "../interfaces/ISaveManager.h"
#include <string>

/**
 * @class SaveManager
 * @brief Full implementation of save/load management
 * 
 * Developer: Nguyen Phuc
 * Status: IMPLEMENTED
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

    // Additional helpers for setting/reading saved data
    void setSaveData(int score, int lives, float posX, float posY);
    int getSavedScore() const;
    int getSavedLives() const;
    float getSavedPosX() const;
    float getSavedPosY() const;

private:
    std::string getSaveFilePath() const;

    static constexpr const char* SAVE_FILE = "mario_save.dat";

    bool m_hasSave;

    // Saved game state
    int m_savedScore;
    int m_savedLives;
    float m_savedPosX;
    float m_savedPosY;
};
