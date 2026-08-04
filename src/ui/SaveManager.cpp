#include "SaveManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

SaveManager::SaveManager()
    : m_hasSave(false), m_savedScore(0), m_savedLives(3), m_savedPosX(100.0f), m_savedPosY(400.0f)
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
    file << "posX=" << m_savedPosX << "\n";
    file << "posY=" << m_savedPosY << "\n";

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
                else if (key == "posX") m_savedPosX = std::stof(value);
                else if (key == "posY") m_savedPosY = std::stof(value);
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

void SaveManager::setSaveData(int score, int lives, float posX, float posY)
{
    m_savedScore = score;
    m_savedLives = lives;
    m_savedPosX = posX;
    m_savedPosY = posY;
}

int SaveManager::getSavedScore() const { return m_savedScore; }
int SaveManager::getSavedLives() const { return m_savedLives; }
float SaveManager::getSavedPosX() const { return m_savedPosX; }
float SaveManager::getSavedPosY() const { return m_savedPosY; }

std::string SaveManager::getSaveFilePath() const
{
    return SAVE_FILE;
}

bool SaveManager::saveFileExists()
{
    std::ifstream file(SAVE_FILE);
    return file.good();
}
