#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <array>

class ISettingsManager;
class ISaveManager;
class ISoundManager;

/**
 * @class InitialsEntryState
 * @brief Screen for entering 5-letter player initials upon reaching a high score.
 */
class InitialsEntryState : public GameState
{
public:
    InitialsEntryState(
        int finalScore,
        std::shared_ptr<ISettingsManager> settings,
        std::shared_ptr<ISaveManager> saveManager,
        const GameConfig& config = GameConfig{},
        std::shared_ptr<ISoundManager> soundManager = nullptr
    );
    ~InitialsEntryState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void submitScore();
    void updateLayout();

    struct SlotVisual
    {
        sf::RectangleShape box;
        sf::Text text;
    };

    int m_score = 0;
    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    std::shared_ptr<ISoundManager> m_soundManager;
    GameConfig m_config;

    sf::Font m_font;
    bool m_fontLoaded = false;

    // 5 characters for initials
    std::array<char, 5> m_letters = { '_', '_', '_', '_', '_' };
    int m_activeSlot = 0; // 0, 1, 2, 3, 4

    float m_cursorBlinkTimer = 0.0f;
    bool m_showCursor = true;

    // Visual shapes & text
    sf::RectangleShape m_background;
    sf::RectangleShape m_frame;
    sf::Text m_titleText{m_font};
    sf::Text m_scoreText{m_font};
    sf::Text m_promptText{m_font};
    sf::Text m_hintText{m_font};

    std::array<SlotVisual, 5> m_slots;
};
