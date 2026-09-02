#pragma once

#include "../interfaces/ISaveManager.h"
#include <string>
#include <vector>
#include <map>
#include <set>

/**
 * @class SaveManager
 * @brief Saves and loads game state, career stats, and achievements.
 *
 * Developer: Nguyen Phuc
 *
 * Saves and loads game state (score, lives, position) to a single file.
 * Tracks lifetime career statistics and an achievement trophy system.
 */
class SaveManager : public ISaveManager
{
public:
    SaveManager();
    ~SaveManager() override;

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

    // Career statistics
    void recordStat(const std::string& type, int amount = 1) override;
    int getStat(const std::string& type) const override;
    void flushStats() override;

    // Achievements
    bool isAchievementUnlocked(const std::string& id) const override;
    void unlockAchievement(const std::string& id) override;
    std::vector<std::pair<std::string, bool>> getAchievementStatuses() const override;
    std::vector<std::pair<std::string, bool>> evaluateAchievements() override;
    std::vector<std::string> drainUnlockedAchievements() override;

private:
    std::string getSaveFilePath() const;
    std::string getHighScoresFilePath() const;
    std::string getProgressionFilePath() const;
    std::string getStatsFilePath() const;
    std::string getAchievementsFilePath() const;
    void loadProgression();
    void loadStats();
    void saveStats();
    void loadAchievements();
    void saveAchievements();
    void checkAndUnlock(const std::string& id, bool condition, std::vector<std::pair<std::string, bool>>& newlyUnlocked);

    static constexpr const char* SAVE_FILE = "savegame.dat";
    static constexpr const char* SCORES_FILE = "scores.dat";
    static constexpr const char* PROGRESSION_FILE = "progression.dat";
    static constexpr const char* STATS_FILE = "stats.dat";
    static constexpr const char* ACHIEVEMENTS_FILE = "achievements.dat";

    bool m_hasSave = false;
    int m_maxUnlockedStage = 1;

    std::map<std::string, int> m_stats;
    std::set<std::string> m_unlockedAchievements;
    std::vector<std::string> m_pendingUnlocks; // Queue for newly unlocked achievement IDs
    bool m_statsDirty = false;
    bool m_achievementsDirty = false;
};
