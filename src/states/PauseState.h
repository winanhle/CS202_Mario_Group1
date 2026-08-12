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
 * Receives a non-owning pointer to the game's SaveManager so "Save & Quit"
 * can persist the current player state before returning to the main menu,
 * and a non-owning pointer to the player so key rebinds made in the menu
 * are applied when the game resumes.
 */
class PauseState : public GameState
{
public:
    explicit PauseState(std::shared_ptr<ISettingsManager> settings,
                        ISaveManager* saveManager,
                        IPlayerManager* player);
    ~PauseState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void saveAndQuitToMenu();

    std::shared_ptr<ISettingsManager> m_settings;
    ISaveManager* m_saveManager; // non-owning; owned by GameWorld
    IPlayerManager* m_player;     // non-owning; owned by GameWorld
    SettingsMenu m_menu;
};