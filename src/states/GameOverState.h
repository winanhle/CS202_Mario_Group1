#pragma once

#include "../core/GameState.h"
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @class GameOverState
 * @brief Game over state
 * 
 * Displays a game over screen with the final score.
 * Press SPACE to return to the main menu.
 */
class GameOverState : public GameState
{
public:
    GameOverState();
    ~GameOverState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    void setFinalScore(int score);

private:
    void returnToMenu();

    sf::Font m_font;
    bool m_fontLoaded;

    sf::Text m_titleText{m_font};
    sf::Text m_scoreText{m_font};
    sf::Text m_promptText{m_font};

    // Blinks the "Press SPACE" prompt
    float m_blinkTimer;
    static constexpr float BLINK_INTERVAL = 0.5f;
    bool m_showPrompt;

    int m_finalScore;
};
