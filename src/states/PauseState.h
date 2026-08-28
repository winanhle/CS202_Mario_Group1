#pragma once

#include "../core/GameState.h"
#include "../ui/SettingsMenu.h"
#include <memory>

class ISettingsManager;
class ISaveManager;
class IPlayerManager;

/**
 * @class PauseState
 * @brief Pause menu state shown when ESC is pressed during gameplay
 * 
 * Opens the settings/pause menu:
 *   - PAUSED context -> root menu (Resume / Settings / Save & Quit / Quit to Menu)
 *   - Settings sub-screen for volume and key bindings
 * 
 * Receives the shared ISaveManager so "Save & Quit" can persist the current
 * player state before returning to the main menu, and non-owning player pointers
 * so key rebinds made in the menu are applied when the game resumes.
 */
class PauseState : public GameState
{
public:
    explicit PauseState(std::shared_ptr<ISettingsManager> settings,
                        std::shared_ptr<ISaveManager> saveManager,
                        IPlayerManager* player1,
                        IPlayerManager* player2 = nullptr);
    ~PauseState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void saveAndQuitToMenu();

    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    IPlayerManager* m_player1;   // non-owning; owned by GameWorld
    IPlayerManager* m_player2;   // non-owning; owned by GameWorld
    SettingsMenu m_menu;
};