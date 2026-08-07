#pragma once

#include "../core/GameState.h"
#include "../ui/SettingsMenu.h"
#include <memory>

class ISettingsManager;

/**
 * @class PauseState
 * @brief Pause menu state shown when ESC is pressed during gameplay
 * 
 * Opens the settings/pause menu:
 *   - PAUSED context -> root menu (Resume / Settings / Quit to Menu)
 *   - Settings sub-screen for volume and key bindings
 */
class PauseState : public GameState
{
public:
    explicit PauseState(std::shared_ptr<ISettingsManager> settings);
    ~PauseState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    std::shared_ptr<ISettingsManager> m_settings;
    SettingsMenu m_menu;
};