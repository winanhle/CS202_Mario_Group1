#pragma once

#include "../core/GameState.h"
#include "../core/GameMemento.h"
#include "../ui/SettingsMenu.h"
#include <memory>
#include <optional>

class ISettingsManager;
class ISaveManager;
class IPlayerManager;
class ISoundManager;

/**
 * @class PauseState
 * @brief Pause menu state shown when ESC is pressed during gameplay
 * 
 * Opens the settings/pause menu:
 *   - PAUSED context -> root menu (Resume / Settings / Save & Quit / Quit to Menu)
 *   - Settings sub-screen for volume and key bindings
 * 
 * Receives the shared ISaveManager and optional GameMemento snapshot so "Save & Quit"
 * can persist the game state before returning to the main menu.
 */
class PauseState : public GameState
{
public:
    explicit PauseState(
        std::shared_ptr<ISettingsManager> settings,
        std::shared_ptr<ISaveManager> saveManager,
        IPlayerManager* player1,
        IPlayerManager* player2 = nullptr,
        std::optional<GameMemento> saveSnapshot = std::nullopt,
        std::shared_ptr<ISoundManager> soundManager = nullptr
    );
    ~PauseState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void saveAndQuitToMenu();

    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    std::shared_ptr<ISoundManager> m_soundManager;
    IPlayerManager* m_player1;   // non-owning; owned by GameWorld
    IPlayerManager* m_player2;   // non-owning; owned by GameWorld
    std::optional<GameMemento> m_saveSnapshot;
    SettingsMenu m_menu;
};