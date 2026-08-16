#include "SettingsManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

SettingsManager::SettingsManager()
{
    resetToDefault();
}

void SettingsManager::resetToDefault()
{
    m_volume = 100.0f;
    // Default key bindings (P1 uses WASD+F, P2 uses Arrows+Period)
    m_keys[static_cast<int>(GameAction::P1Jump)] = sf::Keyboard::Key::Space;
    m_keys[static_cast<int>(GameAction::P1MoveLeft)] = sf::Keyboard::Key::A;
    m_keys[static_cast<int>(GameAction::P1MoveRight)] = sf::Keyboard::Key::D;
    m_keys[static_cast<int>(GameAction::P1Shoot)] = sf::Keyboard::Key::F;
    
    m_keys[static_cast<int>(GameAction::P2Jump)] = sf::Keyboard::Key::Up;
    m_keys[static_cast<int>(GameAction::P2MoveLeft)] = sf::Keyboard::Key::Left;
    m_keys[static_cast<int>(GameAction::P2MoveRight)] = sf::Keyboard::Key::Right;
    m_keys[static_cast<int>(GameAction::P2Shoot)] = sf::Keyboard::Key::Period;
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
            else if (key == "p1Jump")
            {
                m_keys[static_cast<int>(GameAction::P1Jump)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "p1MoveLeft")
            {
                m_keys[static_cast<int>(GameAction::P1MoveLeft)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "p1MoveRight")
            {
                m_keys[static_cast<int>(GameAction::P1MoveRight)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "p1Shoot")
            {
                m_keys[static_cast<int>(GameAction::P1Shoot)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "p2Jump")
            {
                m_keys[static_cast<int>(GameAction::P2Jump)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "p2MoveLeft")
            {
                m_keys[static_cast<int>(GameAction::P2MoveLeft)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "p2MoveRight")
            {
                m_keys[static_cast<int>(GameAction::P2MoveRight)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
            }
            else if (key == "p2Shoot")
            {
                m_keys[static_cast<int>(GameAction::P2Shoot)] = static_cast<sf::Keyboard::Key>(std::stoi(value));
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
    // Reject keys that are reserved for system use and must never be rebound.
    // Escape is hardcoded as the pause/cancel key throughout the UI.
    if (key == sf::Keyboard::Key::Escape ||
        key == sf::Keyboard::Key::Unknown)
        return;

    // Prevent duplicate bindings: if another action already uses this key,
    // swap it to the key we are replacing so no action is ever left unbound.
    const int actionCount = static_cast<int>(GameAction::Count);
    const int targetIdx   = static_cast<int>(action);
    for (int i = 0; i < actionCount; ++i)
    {
        if (i != targetIdx && m_keys[i] == key)
        {
            m_keys[i] = m_keys[targetIdx]; // displaced action gets the old key
            break;
        }
    }

    m_keys[targetIdx] = key;
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
    file << "p1Jump=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P1Jump)]) << "\n";
    file << "p1MoveLeft=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P1MoveLeft)]) << "\n";
    file << "p1MoveRight=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P1MoveRight)]) << "\n";
    file << "p1Shoot=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P1Shoot)]) << "\n";
    
    file << "p2Jump=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P2Jump)]) << "\n";
    file << "p2MoveLeft=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P2MoveLeft)]) << "\n";
    file << "p2MoveRight=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P2MoveRight)]) << "\n";
    file << "p2Shoot=" << static_cast<int>(m_keys[static_cast<int>(GameAction::P2Shoot)]) << "\n";

    file.close();
    std::cout << "[SettingsManager] Settings saved to " << getSettingsFilePath() << std::endl;
}

std::string SettingsManager::getSettingsFilePath() const
{
    return SETTINGS_FILE;
}
