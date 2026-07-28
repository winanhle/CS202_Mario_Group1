#pragma once

#include "../core/GameState.h"
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @class MenuState
 * @brief Main menu state with title screen and Start functionality
 * 
 * Displays the main menu with "SUPER MARIO" title using the SuperMario256 font.
 * Press SPACE to start the game and transition to PlayState.
 */

class MenuState : public GameState
{
public:
    MenuState();
    ~MenuState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    /**
     * @brief Transition to PlayState (called on SPACE press)
     */
    void startGame();

    sf::Font m_font;
    bool m_fontLoaded;
    sf::Text m_titleText{m_font};
    sf::Text m_promptText{m_font};
    // Blinking prompt animation
    float m_blinkTimer;
    static constexpr float BLINK_INTERVAL = 0.5f;
    bool m_showPrompt;
};
