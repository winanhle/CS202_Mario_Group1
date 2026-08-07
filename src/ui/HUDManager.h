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
 *
 * Displays: score, lives, item count, world/level, and an SMB-style
 * countdown timer. Includes small animations (score pop, timer blink
 * when low).
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
    void updateItemCount(int count) override;

private:
    void updateTextStrings();
    void updateScorePop(float deltaTime);
    void updateTimer(float deltaTime);

    sf::Font m_font;

    // Label + value Texts are pairs so the layout is easy to tweak
    sf::Text m_scoreLabel{m_font};
    sf::Text m_livesLabel{m_font};
    sf::Text m_itemsLabel{m_font};
    sf::Text m_worldLabel{m_font};
    sf::Text m_timeLabel{m_font};
    sf::Text m_scoreValue{m_font};
    sf::Text m_livesValue{m_font};
    sf::Text m_itemsValue{m_font};
    sf::Text m_worldValue{m_font};
    sf::Text m_timeValue{m_font};

    // Semi-transparent bar behind the text, set up once in initialize()
    sf::RectangleShape m_hudBar;

    int m_displayScore;
    int m_displayLives;
    int m_displayEnemyCount;
    int m_displayItemCount;

    // SMB-style countdown timer (seconds), ticks down in update()
    float m_timeLeft;
    static constexpr float TIME_START = 300.0f;
    static constexpr float TIME_LOW = 60.0f;
    static constexpr float BLINK_INTERVAL = 0.5f;
    float m_blinkTimer;
    bool m_showTimerText;

    // Score "pop" animation state
    int m_previousScore;
    float m_scorePopTimer;
    static constexpr float SCORE_POP_DURATION = 0.25f;

    bool m_fontLoaded;
};
