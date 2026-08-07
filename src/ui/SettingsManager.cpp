#include "SettingsManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

SettingsManager::SettingsManager()
    : m_volume(100.0f)
{
    // Default key bindings (match PlayerManager's current hardcoded keys)
    m_keys[static_cast<int>(GameAction::Jump)] = sf::Keyboard::Key::Space;
    m_keys[static_cast<int>(GameAction::MoveLeft)] = sf::Keyboard::Key::A;
    m_keys[static_cast<int>(GameAction::MoveRight)] = sf::Keyboard::Key::D;
}

void SettingsManager::initialize()
{
    std::ifstream file(getSettingsFilePath());
    if (!file.is_open())
    {
        std::cout << "[SettingsManager] No settings file found, using defaults." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (!std::getline(iss, key, '=') || !std::getline(iss, value))
            continue;

        try
        {
            if (key == "volume")
            {
                m_volume = std::stof(value);
            }
            else if (key == "jump")
            {
                m_keys[static_cast<int>(GameAction::Jump)] =
                    static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "moveLeft")
            {
                m_keys[static_cast<int>(GameAction::MoveLeft)] =
                    static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "moveRight")
            {
                m_keys[static_cast<int>(GameAction::MoveRight)] =
                    static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "[SettingsManager] Warning: malformed value for '" << key
                      << "': " << e.what() << std::endl;
        }
    }

    file.close();
    std::cout << "[SettingsManager] Settings loaded from " << getSettingsFilePath() << std::endl;
}

float SettingsManager::getVolume() const
{
    return m_volume;
}

void SettingsManager::setVolume(float volume)
{
    m_volume = volume;
    if (m_volume < 0.0f)
        m_volume = 0.0f;
    if (m_volume > 100.0f)
        m_volume = 100.0f;
}

sf::Keyboard::Key SettingsManager::getKey(GameAction action) const
{
    return m_keys[static_cast<int>(action)];
}

void SettingsManager::setKey(GameAction action, sf::Keyboard::Key key)
{
    m_keys[static_cast<int>(action)] = key;
}

void SettingsManager::save()
{
    std::ofstream file(getSettingsFilePath());
    if (!file.is_open())
    {
        std::cerr << "[SettingsManager] ERROR: Could not open settings file for writing: "
                  << getSettingsFilePath() << std::endl;
        return;
    }

    // Keys are stored as their SFML enum integer value so they round-trip
    // reliably across keyboard layouts.
    file << "volume=" << static_cast<int>(m_volume) << "\n";
    file << "jump=" << static_cast<int>(m_keys[static_cast<int>(GameAction::Jump)]) << "\n";
    file << "moveLeft=" << static_cast<int>(m_keys[static_cast<int>(GameAction::MoveLeft)]) << "\n";
    file << "moveRight=" << static_cast<int>(m_keys[static_cast<int>(GameAction::MoveRight)]) << "\n";

    file.close();
    std::cout << "[SettingsManager] Settings saved to " << getSettingsFilePath() << std::endl;
}

std::string SettingsManager::getSettingsFilePath() const
{
    return SETTINGS_FILE;
}
