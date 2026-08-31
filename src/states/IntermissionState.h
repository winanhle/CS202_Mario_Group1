#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

/**
 * @class IntermissionState
 * @brief Stage Clear Intermission screen between levels.
 *
 * Displays:
 * - "STAGE X CLEAR!"
 * - "WORLD 1-(X+1)"
 * - Mario / Luigi icon and remaining lives
 * - Automatically advances after 2.0s or immediately on Space/Enter/Click
 */
class IntermissionState : public GameState
{
public:
    explicit IntermissionState(const GameConfig& config,
                              int currentStage,
                              int nextStage,
                              int sharedLives,
                              std::function<void()> onProceed);
    ~IntermissionState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void proceed();

    GameConfig m_config;
    int m_currentStage = 0;
    int m_nextStage = 0;
    int m_sharedLives = 0;
    std::function<void()> m_onProceed;
    bool m_proceeded = false;

    float m_timer = 0.0f;
    static constexpr float DURATION = 2.2f;

    sf::Font m_font;
    bool m_fontLoaded = false;

    sf::Text m_clearText{m_font};
    sf::Text m_worldText{m_font};
    sf::Text m_livesText{m_font};
    sf::Text m_hintText{m_font};

    sf::Texture m_heroTexture;
    sf::Sprite  m_heroSprite{m_heroTexture};
    bool        m_heroLoaded = false;

    sf::Texture m_hero2Texture;
    sf::Sprite  m_hero2Sprite{m_hero2Texture};
    bool        m_hero2Loaded = false;

    static constexpr float SCREEN_W = 800.0f;
    static constexpr float SCREEN_H = 600.0f;

    // Pre-built background shape to avoid per-frame construction
    sf::RectangleShape m_background{{SCREEN_W, SCREEN_H}};
};
