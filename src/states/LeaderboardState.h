#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class ISettingsManager;
class ISaveManager;

/**
 * @class LeaderboardState
 * @brief Displays top 5 highest scores persisted on the system.
 */
class LeaderboardState : public GameState
{
public:
    LeaderboardState(std::shared_ptr<ISettingsManager> settings,
                     std::shared_ptr<ISaveManager> saveManager = nullptr,
                     const GameConfig& config = GameConfig{});
    ~LeaderboardState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void returnToMenu();

    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    GameConfig m_config;

    sf::Font m_font;
    bool m_fontLoaded = false;

    sf::RectangleShape m_background;
    sf::RectangleShape m_frame;
    sf::Text m_titleText{m_font};
    sf::Text m_headerText{m_font};
    sf::Text m_hintText{m_font};

    struct RowVisuals
    {
        sf::Text rankText;
        sf::Text nameText;
        sf::Text scoreText;
        sf::RectangleShape rowBg;
    };

    std::vector<RowVisuals> m_rows;
};
