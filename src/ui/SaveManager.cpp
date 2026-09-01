#include "SaveManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace {
    std::string formatInitials(const std::string& initials)
    {
        std::string safe = initials.substr(0, 5);
        while (safe.length() < 5)
            safe += ' ';
        return safe;
    }

    std::vector<ScoreEntry> sanitizeAndSortScores(std::vector<ScoreEntry> list)
    {
        std::sort(list.begin(), list.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
        });
        if (list.size() > 5)
            list.resize(5);
        return list;
    }
}

SaveManager::SaveManager()
    : m_hasSave(false)
{
    loadProgression();
}

void SaveManager::initialize()
{
    // Check if a save file already exists
    m_hasSave = std::filesystem::exists(getSaveFilePath());
    loadProgression();
    if (m_hasSave)
    {
        std::cout << "[SaveManager] Save file found: " << getSaveFilePath() << std::endl;
    }
    else
    {
        std::cout << "[SaveManager] No existing save file." << std::endl;
    }
}

bool SaveManager::saveGame(const GameMemento& memento)
{
    std::ofstream file(getSaveFilePath());
    if (!file.is_open())
    {
        std::cerr << "[SaveManager] ERROR: Could not open save file for writing: "
                  << getSaveFilePath() << std::endl;
        return false;
    }

    // Write save data snapshot in key=value format
    file << "numPlayers=" << memento.players.size() << "\n";
    for (size_t i = 0; i < memento.players.size(); ++i) {
        file << "p" << (i+1) << "score=" << memento.players[i].score << "\n";
        file << "p" << (i+1) << "coins=" << memento.players[i].coins << "\n";
    }
    file << "lives=" << memento.lives << "\n";
    file << "level=" << memento.stage << "\n";
    file << "p1Char=" << static_cast<int>(memento.config.player1Character) << "\n";
    file << "p2Char=" << static_cast<int>(memento.config.player2Character) << "\n";
    file << "mode=" << static_cast<int>(memento.config.mode) << "\n";
    if (!memento.config.customMapPath.empty())
    {
        file << "customMap=" << memento.config.customMapPath << "\n";
    }

    file.close();
    m_hasSave = true;

    int totalScore = 0;
    for (const auto& p : memento.players) totalScore += p.score;

    std::cout << "[SaveManager] Game saved successfully (Memento snapshot: Stage "
              << memento.stage << ", Score " << totalScore << ")." << std::endl;
    return true;
}

std::optional<GameMemento> SaveManager::loadGame()
{
    std::ifstream file(getSaveFilePath());
    if (!file.is_open())
    {
        std::cerr << "[SaveManager] ERROR: Could not open save file for reading: "
                  << getSaveFilePath() << std::endl;
        return std::nullopt;
    }

    GameMemento memento;
    bool hasAnyData = false;

    // Parse key=value format
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            try
            {
                if (key == "numPlayers") {
                    memento.players.resize(std::min(2, std::max(0, std::stoi(value))));
                    hasAnyData = true;
                }
                else if (key.rfind("p", 0) == 0 && key.find("score") != std::string::npos) {
                    // Extract player index
                    size_t pIndex = std::stoi(key.substr(1, 1)) - 1;
                    if (pIndex < 2) {
                          if (pIndex >= memento.players.size()) memento.players.resize(pIndex + 1);
                        memento.players[pIndex].score = std::stoi(value);
                        hasAnyData = true;
                    }
                }
                else if (key.rfind("p", 0) == 0 && key.find("coins") != std::string::npos) {
                    size_t pIndex = std::stoi(key.substr(1, 1)) - 1;
                    if (pIndex < 2) {
                          if (pIndex >= memento.players.size()) memento.players.resize(pIndex + 1);
                        memento.players[pIndex].coins = std::stoi(value);
                        hasAnyData = true;
                    }
                }
                else if (key == "lives") { memento.lives = std::stoi(value); hasAnyData = true; }
                else if (key == "level") { memento.stage = std::stoi(value); hasAnyData = true; }
                else if (key == "p1Char") { memento.config.player1Character = static_cast<CharacterType>(std::stoi(value)); hasAnyData = true; }
                else if (key == "p2Char") { memento.config.player2Character = static_cast<CharacterType>(std::stoi(value)); hasAnyData = true; }
                else if (key == "mode") { memento.config.mode = static_cast<GameMode>(std::stoi(value)); hasAnyData = true; }
                else if (key == "customMap") { memento.config.customMapPath = value; hasAnyData = true; }
            }
            catch (const std::exception& e)
            {
                std::cerr << "[SaveManager] Warning: malformed value for '" << key
                          << "': " << e.what() << std::endl;
            }
        }
    }

    file.close();
    if (!hasAnyData)
    {
        std::cerr << "[SaveManager] Save file was empty or corrupted." << std::endl;
        return std::nullopt;
    }

    m_hasSave = true;
    std::cout << "[SaveManager] Game loaded successfully (Memento snapshot: Stage "
              << memento.stage << ", Score " << memento.getTotalScore() << ")." << std::endl;
    return memento;
}

bool SaveManager::hasSaveFile() const
{
    return m_hasSave;
}

void SaveManager::deleteSave()
{
    std::error_code ec;
    std::filesystem::remove(getSaveFilePath(), ec);
    m_hasSave = false;
    std::cout << "[SaveManager] Save file deleted." << std::endl;
}

std::string SaveManager::getSaveFilePath() const
{
    return SAVE_FILE;
}

std::string SaveManager::getHighScoresFilePath() const
{
    return SCORES_FILE;
}

std::string SaveManager::getProgressionFilePath() const
{
    return PROGRESSION_FILE;
}

void SaveManager::loadProgression()
{
    std::ifstream file(getProgressionFilePath());
    if (file.is_open())
    {
        int stage = 1;
        if (file >> stage)
        {
            m_maxUnlockedStage = std::max(1, stage);
        }
        file.close();
    }
}

int SaveManager::getMaxUnlockedStage() const
{
    return m_maxUnlockedStage;
}

void SaveManager::unlockStage(int stageNumber)
{
    if (stageNumber <= 0) return;
    if (stageNumber > m_maxUnlockedStage)
    {
        int previousMax = m_maxUnlockedStage;
        m_maxUnlockedStage = stageNumber;
        std::ofstream file(getProgressionFilePath());
        if (file.is_open())
        {
            file << m_maxUnlockedStage << "\n";
            file.close();
            std::cout << "[SaveManager] Stage progression updated! Max unlocked stage: "
                      << m_maxUnlockedStage << std::endl;
        }
        else
        {
            std::cerr << "[SaveManager] ERROR: Could not open progression file for writing: "
                      << getProgressionFilePath() << std::endl;
            m_maxUnlockedStage = previousMax; // Rollback in-memory cache if file write failed
        }
    }
}

std::vector<ScoreEntry> SaveManager::loadHighScores() const
{
    std::vector<ScoreEntry> list;
    std::ifstream file(getHighScoresFilePath());
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty()) continue;
            std::istringstream iss(line);
            ScoreEntry entry;
            if (iss >> entry.initials >> entry.score)
            {
                entry.initials = formatInitials(entry.initials);
                list.push_back(entry);
            }
        }
        file.close();
    }

    // If file was missing or had fewer than 5 entries, fill with defaults
    if (list.size() < 5)
    {
        const std::vector<ScoreEntry> defaults = {
            { "MARIO", 10000 },
            { "LUIGI", 8000 },
            { "PEACH", 6000 },
            { "TOADS", 4000 },
            { "YOSHI", 2000 }
        };

        for (const auto& def : defaults)
        {
            if (list.size() >= 5) break;
            list.push_back(def);
        }
    }

    return sanitizeAndSortScores(std::move(list));
}

void SaveManager::saveHighScores(const std::vector<ScoreEntry>& entries)
{
    std::vector<ScoreEntry> sorted = sanitizeAndSortScores(entries);

    std::ofstream file(getHighScoresFilePath());
    if (file.is_open())
    {
        for (const auto& entry : sorted)
        {
            file << formatInitials(entry.initials) << " " << entry.score << "\n";
        }
        file.close();
    }
    else
    {
        std::cerr << "[SaveManager] ERROR: Could not open high scores file for writing: "
                  << getHighScoresFilePath() << "\n";
    }
}

bool SaveManager::isHighScore(int score) const
{
    if (score <= 0) return false;
    auto list = loadHighScores();
    if (list.size() < 5) return true;
    return score > list.back().score;
}

void SaveManager::addHighScore(const std::string& initials, int score)
{
    auto list = loadHighScores();
    list.push_back({ formatInitials(initials), score });
    saveHighScores(list);
}