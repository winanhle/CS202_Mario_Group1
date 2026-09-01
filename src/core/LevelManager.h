#pragma once

#include <string>
#include <vector>

/**
 * @class LevelManager
 * @brief Discovers, orders, and tracks progression through game levels.
 *
 * Convention: map files are named stage1.tmx, stage2.tmx, ... in the map directory.
 * Also supports folder-based layout: stage1/stage1.tmx, stage2/stage2.tmx, etc.
 *
 * OOP Principles:
 *   SRP — only responsible for level file discovery and ordering
 *   OCP — new levels added by dropping files, no code changes needed
 */
class LevelManager {
public:
    LevelManager() = default;
    ~LevelManager() = default;

    /**
     * @brief Scan the map directory for stage files.
     * Discovers stageN.tmx files (flat) and stageN/ folders (containing .tmx).
     * Sorts results by stage number N for consecutive level ordering.
     * @param mapDirectory Path to the map assets directory (default: "assets/map/")
     */
    void discoverLevels(const std::string& mapDirectory = "assets/map/");

    /// Get the file path for the current level
    std::string getCurrentLevelPath() const;

    /// Get the current level number (1-based, for display)
    int getCurrentLevelNumber() const;

    /// Advance to the next level. Returns false if no more levels exist.
    bool advanceToNextLevel();

    /// Check if the current level is the last one
    bool isLastLevel() const;

    /// Reset back to level 1
    void reset();

    /// Get total number of discovered levels
    int getTotalLevels() const;

    /// Set the current level by index (0-based internal)
    void setCurrentLevel(int index);

    /// Set the current level by matching map file path or filename
    bool setLevelByPath(const std::string& path);

private:
    std::vector<std::string> m_levelPaths;  // ordered list of .tmx file paths
    int m_currentIndex = 0;
};
