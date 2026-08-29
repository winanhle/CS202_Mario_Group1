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
}

void SaveManager::initialize()
{
    // Check if a save file already exists
    std::ifstream file(getSaveFilePath());
    m_hasSave = file.good();
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
    file << "score=" << memento.score << "\n";
    file << "lives=" << memento.lives << "\n";
    file << "level=" << memento.stage << "\n";
    file << "coins=" << memento.coins << "\n";
    file << "p1Char=" << static_cast<int>(memento.config.player1Character) << "\n";
    file << "p2Char=" << static_cast<int>(memento.config.player2Character) << "\n";
    file << "mode=" << static_cast<int>(memento.config.mode) << "\n";
    if (!memento.config.customMapPath.empty())
    {
        file << "customMap=" << memento.config.customMapPath << "\n";
    }

    file.close();
    m_hasSave = true;

    std::cout << "[SaveManager] Game saved successfully (Memento snapshot: Stage "
              << memento.stage << ", Score " << memento.score << ")." << std::endl;
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
                if (key == "score") { memento.score = std::stoi(value); hasAnyData = true; }
                else if (key == "lives") { memento.lives = std::stoi(value); hasAnyData = true; }
                else if (key == "level") { memento.stage = std::stoi(value); hasAnyData = true; }
                else if (key == "coins") { memento.coins = std::stoi(value); hasAnyData = true; }
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
              << memento.stage << ", Score " << memento.score << ")." << std::endl;
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