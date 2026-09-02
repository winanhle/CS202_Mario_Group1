#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include "../ui/UIUtils.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <array>

class ISettingsManager;
class ISaveManager;
class ISoundManager;

/**
 * @class WinState
 * @brief Victory / Game Clear state
 *
 * Displays a celebratory victory screen when the player completes all levels:
 * - Princess Peach + Hero celebratory sprites
 * - Score breakdown (Base score + Lives bonus = Final Total Score)
 * - Interactive options (PLAY AGAIN & MAIN MENU) via UINavigator
 */
class WinState : public GameState
{
public:
    explicit WinState(
        std::shared_ptr<ISettingsManager> settings,
        std::shared_ptr<ISaveManager> saveManager,
        const GameConfig& config,
        int baseScore,
        int livesRemaining,
        std::shared_ptr<ISoundManager> soundManager = nullptr
    );
    ~WinState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void refreshUI();
    void confirmSelection();
    void playAgain();
    void returnToMenu();

    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    std::shared_ptr<ISoundManager> m_soundManager;
    bool m_winSoundPlayed = false;
    GameConfig m_config;

    int m_baseScore = 0;
    int m_livesRemaining = 0;
    int m_totalScore = 0;

    sf::Font m_font;
    bool m_fontLoaded = false;

    sf::Text m_titleText{m_font};
    sf::Text m_subtitleText{m_font};
    sf::Text m_scoreLabelText{m_font};
    sf::Text m_livesBonusText{m_font};
    sf::Text m_totalScoreText{m_font};
    sf::Text m_hintText{m_font};
    sf::Text m_playAgainText{m_font};
    sf::Text m_menuText{m_font};

    // Sprites: Peach and Hero(es)
    sf::Texture m_peachTexture;
    sf::Sprite  m_peachSprite{m_peachTexture};
    bool        m_peachLoaded = false;

    // Peach speech bubble
    sf::RectangleShape m_bubbleBox{{224.f, 32.f}};
    sf::ConvexShape    m_bubbleTail;
    sf::Text           m_bubbleText{m_font};

    sf::Texture m_heroTexture;
    sf::Sprite  m_heroSprite{m_heroTexture};
    bool        m_heroLoaded = false;

    sf::Texture m_hero2Texture;
    sf::Sprite  m_hero2Sprite{m_hero2Texture};
    bool        m_hero2Loaded = false;

    static constexpr int OPTION_COUNT = 2;
    UINavigator m_nav{OPTION_COUNT};
    std::array<sf::RectangleShape, OPTION_COUNT> m_optionCards;

    sf::Text& getOptionText(int index) { return (index == 0) ? m_playAgainText : m_menuText; }
    const sf::Text& getOptionText(int index) const { return (index == 0) ? m_playAgainText : m_menuText; }

    float m_animTimer = 0.0f;
    sf::Vector2u m_windowSize{800u, 600u};

    static constexpr float WIN_W = 800.0f;
    static constexpr float WIN_H = 600.0f;

    // Pre-built background shapes (avoid per-frame construction in render)
    sf::RectangleShape m_background{{WIN_W, WIN_H}};
    sf::RectangleShape m_frame{{WIN_W - 60.0f, WIN_H - 60.0f}};
};
