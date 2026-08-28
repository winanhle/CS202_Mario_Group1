#include "LeaderboardState.h"
#include "MenuState.h"
#include "../core/StateManager.h"
#include <iomanip>
#include <sstream>
#include <iostream>

static constexpr sf::Color GOLD_COLOR{ 255, 215, 0 };
static constexpr sf::Color SILVER_COLOR{ 210, 215, 225 };
static constexpr sf::Color BRONZE_COLOR{ 215, 140, 60 };
static constexpr sf::Color WHITE_COLOR{ 240, 240, 240 };
static constexpr sf::Color GRAY_COLOR{ 160, 160, 160 };

LeaderboardState::LeaderboardState(std::shared_ptr<ISettingsManager> settings,
                                   const GameConfig& config)
    : m_settings(std::move(settings))
    , m_config(config)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    if (!m_fontLoaded)
    {
        std::cerr << "[LeaderboardState] ERROR: Failed to open font assets/fonts/SuperMario256.ttf\n";
    }

    m_background.setSize({ 800.0f, 600.0f });
    m_background.setFillColor(sf::Color(10, 10, 25, 250));

    m_frame.setSize({ 660.0f, 480.0f });
    m_frame.setFillColor(sf::Color(0, 0, 0, 190));
    m_frame.setOutlineColor(GOLD_COLOR);
    m_frame.setOutlineThickness(3.0f);
    m_frame.setPosition({ 70.0f, 60.0f });

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("HALL OF FAME");
        m_titleText.setCharacterSize(36);
        m_titleText.setFillColor(GOLD_COLOR);
        centerOrigin(m_titleText);
        m_titleText.setPosition({ 400.0f, 110.0f });

        m_headerText.setFont(m_font);
        m_headerText.setString("RANK     NAME       SCORE");
        m_headerText.setCharacterSize(20);
        m_headerText.setFillColor(GRAY_COLOR);
        centerOrigin(m_headerText);
        m_headerText.setPosition({ 400.0f, 165.0f });

        m_hintText.setFont(m_font);
        m_hintText.setString("PRESS ENTER / SPACE / ESC TO RETURN");
        m_hintText.setCharacterSize(16);
        m_hintText.setFillColor(GRAY_COLOR);
        centerOrigin(m_hintText);
        m_hintText.setPosition({ 400.0f, 500.0f });

        auto scores = SaveManager::getHighScores();
        const float startY = 205.0f;
        const float rowHeight = 52.0f;

        const std::array<std::string, 5> rankLabels = { "1ST", "2ND", "3RD", "4TH", "5TH" };

        for (std::size_t i = 0; i < scores.size() && i < 5; ++i)
        {
            RowVisuals row{ sf::Text(m_font), sf::Text(m_font), sf::Text(m_font), sf::RectangleShape() };

            float y = startY + static_cast<float>(i) * rowHeight;

            sf::Color rowColor = WHITE_COLOR;
            if (i == 0) rowColor = GOLD_COLOR;
            else if (i == 1) rowColor = SILVER_COLOR;
            else if (i == 2) rowColor = BRONZE_COLOR;

            row.rowBg.setSize({ 580.0f, 44.0f });
            row.rowBg.setOrigin({ 290.0f, 22.0f });
            row.rowBg.setPosition({ 400.0f, y });
            row.rowBg.setFillColor(sf::Color(30, 30, 45, 140));
            row.rowBg.setOutlineColor(sf::Color(80, 80, 100, 100));
            row.rowBg.setOutlineThickness(1.0f);

            // Rank
            row.rankText.setFont(m_font);
            row.rankText.setString(rankLabels[i]);
            row.rankText.setCharacterSize(22);
            row.rankText.setFillColor(rowColor);
            centerOrigin(row.rankText);
            row.rankText.setPosition({ 200.0f, y });

            // Name
            row.nameText.setFont(m_font);
            row.nameText.setString(scores[i].initials);
            row.nameText.setCharacterSize(22);
            row.nameText.setFillColor(rowColor);
            centerOrigin(row.nameText);
            row.nameText.setPosition({ 380.0f, y });

            // Score formatted to 6 digits
            std::string scoreStr = std::to_string(scores[i].score);
            if (scoreStr.length() < 6)
                scoreStr = std::string(6 - scoreStr.length(), '0') + scoreStr;

            row.scoreText.setFont(m_font);
            row.scoreText.setString(scoreStr);
            row.scoreText.setCharacterSize(22);
            row.scoreText.setFillColor(rowColor);
            centerOrigin(row.scoreText);
            row.scoreText.setPosition({ 570.0f, y });

            m_rows.push_back(std::move(row));
        }
    }
}

void LeaderboardState::handleInput(const sf::Event& event)
{
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>())
    {
        switch (keyPress->code)
        {
        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:
        case sf::Keyboard::Key::Escape:
            returnToMenu();
            break;
        default:
            break;
        }
    }
    else if (event.is<sf::Event::MouseButtonPressed>())
    {
        returnToMenu();
    }
}

void LeaderboardState::returnToMenu()
{
    if (auto* manager = getStateManager())
    {
        manager->changeState(std::make_unique<MenuState>(m_settings));
    }
}

void LeaderboardState::update(float deltaTime)
{
    (void)deltaTime;
}

void LeaderboardState::render(sf::RenderWindow& window) const
{
    if (!m_fontLoaded)
        return;

    window.draw(m_background);
    window.draw(m_frame);
    window.draw(m_titleText);
    window.draw(m_headerText);

    for (const auto& row : m_rows)
    {
        window.draw(row.rowBg);
        window.draw(row.rankText);
        window.draw(row.nameText);
        window.draw(row.scoreText);
    }

    window.draw(m_hintText);
}
