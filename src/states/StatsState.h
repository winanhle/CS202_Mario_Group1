#pragma once

#include "../core/GameState.h"
#include "../core/GameConfig.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include <utility>

class ISettingsManager;
class ISaveManager;
class ISoundManager;

/**
 * @class StatsState
 * @brief Records & Achievements viewing screen.
 *
 * Displays career statistics and achievement trophies
 * accessible from the main menu (Press R).
 */
class StatsState : public GameState
{
public:
    explicit StatsState(
        std::shared_ptr<ISettingsManager> settings,
        std::shared_ptr<ISaveManager> saveManager = nullptr,
        std::shared_ptr<ISoundManager> soundManager = nullptr
    );
    ~StatsState() override = default;

    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

private:
    void returnToMenu();
    void setupAchievements();

    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager> m_saveManager;
    std::shared_ptr<ISoundManager> m_soundManager;

    sf::Font m_font;
    bool m_fontLoaded = false;

    // Background
    sf::RectangleShape m_background;
    sf::RectangleShape m_frame;

    // Title
    sf::Text m_titleText{m_font};
    sf::Text m_hintText{m_font};

    // Stats section
    sf::Text m_statsHeaderText{m_font};
    struct StatRow
    {
        std::string labelStr;
        std::string valueStr;
    };
    std::vector<StatRow> m_statRows;

    // Achievements section
    sf::Text m_achieveHeaderText{m_font};
    struct TrophyCard
    {
        sf::RectangleShape card;
        std::string nameStr;
        std::string descStr;
        std::string iconStr;
        bool unlocked = false;
    };
    std::vector<TrophyCard> m_trophies;
    int m_currentPage = 0;
    static constexpr int ACHIEVEMENTS_PER_PAGE = 6;
    static constexpr int COLS = 3;
    static constexpr int ROWS = 2;

    static constexpr float WIN_W = 800.0f;
    static constexpr float WIN_H = 600.0f;
};
