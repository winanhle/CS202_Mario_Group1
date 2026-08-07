#pragma once

#include "../interfaces/ISettingsManager.h"
#include <string>

/**
 * @class SettingsManager
 * @brief Stores user settings (volume, key bindings) and persists them to a file.
 *
 * Developer: Nguyen Phuc
 *
 * Single Responsibility: settings data + persistence only. No rendering,
 * no game logic. Uses a simple text-based key=value format for easy
 * debugging (mirrors SaveManager's pattern).
 */
class SettingsManager : public ISettingsManager
{
public:
    SettingsManager();
    ~SettingsManager() override = default;

    void initialize() override;

    float getVolume() const override;
    void setVolume(float volume) override;

    sf::Keyboard::Key getKey(GameAction action) const override;
    void setKey(GameAction action, sf::Keyboard::Key key) override;

    void save() override;

private:
    std::string getSettingsFilePath() const;

    static constexpr const char* SETTINGS_FILE = "settings.ini";

    float m_volume;
    sf::Keyboard::Key m_keys[static_cast<int>(GameAction::Count)];
};
