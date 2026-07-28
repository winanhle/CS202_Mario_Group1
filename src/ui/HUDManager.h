#pragma once

#include "../interfaces/IHUDManager.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @class HUDManager
 * @brief Full implementation of HUD/UI management
 * 
 * Developer: Nguyen Phuc
 * Status: IMPLEMENTED
 * 
 * Displays score, lives, and enemy count using SuperMario256 font.
 * Renders HUD elements as text overlay on the game window.
 */
class HUDManager : public IHUDManager
{
public:
    HUDManager();
    ~HUDManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    void updateScore(int score) override;
    void updateLives(int lives) override;
    void updateEnemyCount(int count) override;

private:
    void updateTextStrings();

    sf::Font m_font;

    // Labels (constructed with font reference)
    sf::Text m_scoreLabel{m_font};
    sf::Text m_livesLabel{m_font};
    sf::Text m_enemyLabel{m_font};

    // Values (constructed with font reference)
    sf::Text m_scoreValue{m_font};
    sf::Text m_livesValue{m_font};
    sf::Text m_enemyValue{m_font};

    // Pre-configured HUD background bar (created once in initialize)
    sf::RectangleShape m_hudBar;

    int m_displayScore;
    int m_displayLives;
    int m_displayEnemyCount;

    bool m_fontLoaded;
};
