#include "LevelManager.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>

namespace fs = std::filesystem;

void LevelManager::discoverLevels(const std::string& mapDirectory) {
    m_levelPaths.clear();
    m_currentIndex = 0;

    if (!fs::exists(mapDirectory) || !fs::is_directory(mapDirectory)) {
        std::cerr << "[LevelManager] Map directory not found: " << mapDirectory << std::endl;
        return;
    }

    // Collect (stageNumber, filePath) pairs for sorting
    std::vector<std::pair<int, std::string>> discovered;

    // Pattern: stageN.tmx (case-insensitive)
    std::regex flatPattern(R"(stage(\d+)\.tmx)", std::regex::icase);

    for (const auto& entry : fs::directory_iterator(mapDirectory)) {
        if (entry.is_regular_file()) {
            // Check for flat stageN.tmx files
            std::string filename = entry.path().filename().string();
            std::smatch match;
            if (std::regex_match(filename, match, flatPattern)) {
                int stageNum = std::stoi(match[1].str());
                discovered.push_back({stageNum, entry.path().string()});
            }
        } else if (entry.is_directory()) {
            // Check for folder-based layout: stageN/stageN.tmx or stageN/*.tmx
            std::string dirName = entry.path().filename().string();
            std::smatch match;
            std::regex dirPattern(R"(stage(\d+))", std::regex::icase);
            if (std::regex_match(dirName, match, dirPattern)) {
                int stageNum = std::stoi(match[1].str());
                // Look for stageN.tmx inside the folder first
                fs::path expectedFile = entry.path() / (dirName + ".tmx");
                if (fs::exists(expectedFile)) {
                    discovered.push_back({stageNum, expectedFile.string()});
                } else {
                    // Fallback: find any .tmx file in the folder
                    for (const auto& subEntry : fs::directory_iterator(entry.path())) {
                        if (subEntry.is_regular_file() &&
                            subEntry.path().extension() == ".tmx") {
                            discovered.push_back({stageNum, subEntry.path().string()});
                            break;
                        }
                    }
                }
            }
        }
    }

    // Sort by stage number
    std::sort(discovered.begin(), discovered.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    // Extract sorted paths
    for (const auto& [num, path] : discovered) {
        m_levelPaths.push_back(path);
    }

    std::cout << "[LevelManager] Discovered " << m_levelPaths.size() << " level(s):" << std::endl;
    for (size_t i = 0; i < m_levelPaths.size(); ++i) {
        std::cout << "  Stage " << (i + 1) << ": " << m_levelPaths[i] << std::endl;
    }
}

std::string LevelManager::getCurrentLevelPath() const {
    if (m_levelPaths.empty() || m_currentIndex < 0 ||
        m_currentIndex >= static_cast<int>(m_levelPaths.size())) {
        std::cerr << "[LevelManager] No levels available!" << std::endl;
        return "";
    }
    return m_levelPaths[m_currentIndex];
}

int LevelManager::getCurrentLevelNumber() const {
    return m_currentIndex + 1;
}

bool LevelManager::advanceToNextLevel() {
    if (isLastLevel()) {
        return false;  // no more levels
    }
    ++m_currentIndex;
    return true;
}

bool LevelManager::isLastLevel() const {
    return m_currentIndex >= static_cast<int>(m_levelPaths.size()) - 1;
}

void LevelManager::reset() {
    m_currentIndex = 0;
}

int LevelManager::getTotalLevels() const {
    return static_cast<int>(m_levelPaths.size());
}

void LevelManager::setCurrentLevel(int index) {
    if (index >= 0 && index < static_cast<int>(m_levelPaths.size())) {
        m_currentIndex = index;
    }
}

bool LevelManager::setLevelByPath(const std::string& path) {
    if (path.empty()) return false;
    std::string targetFilename = fs::path(path).filename().string();
    for (size_t i = 0; i < m_levelPaths.size(); ++i) {
        std::string levelFilename = fs::path(m_levelPaths[i]).filename().string();
        if (levelFilename == targetFilename) {
            m_currentIndex = static_cast<int>(i);
            return true;
        }
    }
    return false;
}
