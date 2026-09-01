#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include "../ui/UINavigator.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <array>

class ISettingsManager;
class ISaveManager;
class ISoundManager;

/**
 * @class GameOverState
 * @brief Interactive game over state
 * 
 * Displays game over screen with the final score and interactive options:
 * - RETRY (restarts the game with the same character & mode configuration)
 * - MAIN MENU (returns to the title screen)
 * 
 * Supports both Keyboard (Up/Down/W/S/Enter/Space) and Mouse navigation.
 */
class GameOverState : public GameState
{
public:
    explicit GameOverState(
        std::shared_ptr<ISettingsManager> settings,
        std::shared_ptr<ISaveManager> saveManager,
        const GameConfig& config = GameConfig{},
        std::shared_ptr<ISoundManager> soundManager = nullptr
    );
    ~GameOverState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    void setFinalScore(int score);

private:
    void refreshUI();
    void confirmSelection();
    void retryGame();
    void returnToMenu();

    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    std::shared_ptr<ISoundManager> m_soundManager;
    GameConfig m_config;

    sf::Font m_font;
    bool m_fontLoaded;

    sf::Text m_titleText{m_font};
    sf::Text m_scoreText{m_font};
    sf::Text m_hintText{m_font};
    sf::Text m_retryText{m_font};
    sf::Text m_menuText{m_font};

    sf::Text& getOptionText(int index) { return (index == 0) ? m_retryText : m_menuText; }
    const sf::Text& getOptionText(int index) const { return (index == 0) ? m_retryText : m_menuText; }

    // Menu options: 0 = RETRY, 1 = MAIN MENU
    static constexpr int OPTION_COUNT = 2;
    UINavigator m_nav{OPTION_COUNT};

    std::array<sf::RectangleShape, OPTION_COUNT> m_optionCards;

    // Animations
    float m_animTimer = 0.0f;
    int   m_finalScore = 0;

    mutable sf::Vector2u m_windowSize{800u, 600u};
    static constexpr float WIN_W = 800.0f;
    static constexpr float WIN_H = 600.0f;
};
