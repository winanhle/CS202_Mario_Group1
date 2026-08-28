#include "SaveManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

SaveManager::SaveManager()
    : m_hasSave(false), m_savedScore(0), m_savedLives(3), m_savedLevel(1), m_savedCoins(0)
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

bool SaveManager::saveGame()
{
    std::ofstream file(getSaveFilePath());
    if (!file.is_open())
    {
        std::cerr << "[SaveManager] ERROR: Could not open save file for writing: "
                  << getSaveFilePath() << std::endl;
        return false;
    }

    // Write save data in a simple key=value format
    file << "score=" << m_savedScore << "\n";
    file << "lives=" << m_savedLives << "\n";
    file << "level=" << m_savedLevel << "\n";
    file << "coins=" << m_savedCoins << "\n";
    file << "p1Char=" << m_savedP1Char << "\n";
    file << "p2Char=" << m_savedP2Char << "\n";
    file << "mode=" << m_savedMode << "\n";

    file.close();
    m_hasSave = true;

    std::cout << "[SaveManager] Game saved successfully." << std::endl;
    return true;
}

bool SaveManager::loadGame()
{
    std::ifstream file(getSaveFilePath());
    if (!file.is_open())
    {
        std::cerr << "[SaveManager] ERROR: Could not open save file for reading: "
                  << getSaveFilePath() << std::endl;
        return false;
    }

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
                if (key == "score") m_savedScore = std::stoi(value);
                else if (key == "lives") m_savedLives = std::stoi(value);
                else if (key == "level") m_savedLevel = std::stoi(value);
                else if (key == "coins") m_savedCoins = std::stoi(value);
                else if (key == "p1Char") m_savedP1Char = std::stoi(value);
                else if (key == "p2Char") m_savedP2Char = std::stoi(value);
                else if (key == "mode") m_savedMode = std::stoi(value);
            }
            catch (const std::exception& e)
            {
                std::cerr << "[SaveManager] Warning: malformed value for '" << key
                          << "': " << e.what() << std::endl;
            }
        }
    }

    file.close();
    m_hasSave = true;

    std::cout << "[SaveManager] Game loaded successfully." << std::endl;
    return true;
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

void SaveManager::setSaveData(int score, int lives, int level, int coins)
{
    m_savedScore = score;
    m_savedLives = lives;
    m_savedLevel = level;
    m_savedCoins = coins;
}

int SaveManager::getSavedScore() const { return m_savedScore; }
int SaveManager::getSavedLives() const { return m_savedLives; }
int SaveManager::getSavedLevel() const { return m_savedLevel; }
int SaveManager::getSavedCoins() const { return m_savedCoins; }

void SaveManager::setGameConfig(int p1Char, int p2Char, int mode)
{
    m_savedP1Char = p1Char;
    m_savedP2Char = p2Char;
    m_savedMode = mode;
}

int SaveManager::getSavedP1Char() const { return m_savedP1Char; }
int SaveManager::getSavedP2Char() const { return m_savedP2Char; }
int SaveManager::getSavedMode() const { return m_savedMode; }

std::string SaveManager::getSaveFilePath() const
{
    return SAVE_FILE;
}

std::string SaveManager::getHighScoresFilePath()
{
    return SCORES_FILE;
}

bool SaveManager::saveFileExists()
{
    std::ifstream file(SAVE_FILE);
    return file.good();
}

std::vector<ScoreEntry> SaveManager::loadHighScores()
{
    return getHighScores();
}

void SaveManager::saveHighScores(const std::vector<ScoreEntry>& entries)
{
    setHighScores(entries);
}

bool SaveManager::isHighScore(int score) const
{
    return checkIsHighScore(score);
}

std::vector<ScoreEntry> SaveManager::getHighScores()
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
                entry.initials = entry.initials.substr(0, 5);
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

    // Sort descending by score
    std::sort(list.begin(), list.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });

    if (list.size() > 5)
        list.resize(5);

    return list;
}

void SaveManager::setHighScores(const std::vector<ScoreEntry>& entries)
{
    std::vector<ScoreEntry> sorted = entries;
    std::sort(sorted.begin(), sorted.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });

    if (sorted.size() > 5)
        sorted.resize(5);

    std::ofstream file(getHighScoresFilePath());
    if (file.is_open())
    {
        for (const auto& entry : sorted)
        {
            file << entry.initials << " " << entry.score << "\n";
        }
        file.close();
    }
    else
    {
        std::cerr << "[SaveManager] ERROR: Could not open high scores file for writing: "
                  << getHighScoresFilePath() << "\n";
    }
}

bool SaveManager::checkIsHighScore(int score)
{
    if (score <= 0) return false;
    auto list = getHighScores();
    if (list.size() < 5) return true;
    return score > list.back().score;
}

void SaveManager::addHighScore(const std::string& initials, int score)
{
    std::string safeInitials = initials.substr(0, 5);
    while (safeInitials.length() < 5)
        safeInitials += ' ';
    auto list = getHighScores();
    list.push_back({ safeInitials, score });
    setHighScores(list);
}