#pragma once

#include "../core/GameState.h"
#include "../ui/SettingsMenu.h"
#include "../ui/MenuBackground.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class ISettingsManager;

/**
 * @class MenuState
 * @brief Main menu state with title screen and Start functionality
 * 
 * Displays the main menu with "SUPER MARIO" title using the SuperMario256 font.
 * Press SPACE to start the game and transition to PlayState.
 * Press ESC to open the settings menu.
 */

class MenuState : public GameState
{
public:
    explicit MenuState(std::shared_ptr<ISettingsManager> settings);
    ~MenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void startGame(bool loadSave = false);

    sf::Font m_font;
    bool m_fontLoaded;
    sf::Text m_titleText{m_font};
    sf::Text m_promptText{m_font};
    sf::Text m_continueText{m_font};
    bool m_hasSave;

    // Blinks the "Press SPACE to start" prompt
    float m_blinkTimer;
    static constexpr float BLINK_INTERVAL = 0.5f;
    bool m_showPrompt;

    // Settings menu (opened with ESC)
    // m_settings must be declared before m_settingsMenu so it is
    // initialized first (m_settingsMenu is constructed with *m_settings)
    std::shared_ptr<ISettingsManager> m_settings;
    SettingsMenu m_settingsMenu;
    bool m_inSettings;
    
    MenuBackground m_bg;
};