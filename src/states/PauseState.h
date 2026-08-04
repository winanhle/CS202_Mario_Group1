#pragma once

#include "../core/GameState.h"
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @class PauseState
 * @brief Game pause state
 * 
 * Displays a pause menu overlay.
 * Pushed on top of PlayState so the game world stays alive underneath and
 * can be resumed by popping this state.
 */
class PauseState : public GameState
{
public:
    PauseState();
    ~PauseState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void resumeGame();

    sf::Font m_font;
    bool m_fontLoaded;

    // m_font must be declared before these Texts so it's initialized first
    sf::Text m_pauseTitle{m_font};
    sf::Text m_promptText{m_font};

    // Blinks the "Press P to resume" prompt
    float m_blinkTimer;
    static constexpr float BLINK_INTERVAL = 0.5f;
    bool m_showPrompt;
};
