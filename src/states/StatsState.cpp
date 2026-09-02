#include "../ui/UIUtils.h"
#include "StatsState.h"
#include "MenuState.h"
#include "../core/StateManager.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/ISoundManager.h"
#include "../data/AchievementDefs.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

namespace
{
    constexpr float WIN_W = 800.0f; // Assuming WIN_W and WIN_H are defined elsewhere, but wait, they aren't in this namespace.
    // Let's just put the card constants here
    constexpr sf::Color BG_COLOR{10, 10, 25};
    constexpr sf::Color FRAME_FILL{0, 0, 0, 190};
    constexpr sf::Color GOLD_COLOR{255, 215, 0};
    constexpr sf::Color WHITE_COLOR{240, 240, 240};
    constexpr sf::Color GRAY_COLOR{160, 160, 160};
    constexpr sf::Color UNLOCKED_BG{40, 50, 30, 200};
    constexpr sf::Color LOCKED_BG{30, 30, 35, 150};
    constexpr sf::Color UNLOCKED_OUTLINE{200, 180, 50};
    constexpr sf::Color LOCKED_OUTLINE{60, 60, 70};

    constexpr float CARD_W = 210.0f;
    constexpr float CARD_H = 85.0f;
    constexpr float GAP_X = 15.0f;
    constexpr float GAP_Y = 10.0f;
    constexpr float START_Y = 325.0f;
}

StatsState::StatsState(
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    std::shared_ptr<ISoundManager> soundManager
)
    : m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_soundManager(std::move(soundManager))
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    if (!m_fontLoaded)
    {
        std::cerr << "[StatsState] ERROR: Failed to open font assets/fonts/SuperMario256.ttf\n";
    }

    m_background.setSize({ WIN_W, WIN_H });
    m_background.setFillColor(BG_COLOR);

    m_frame.setSize({ 700.0f, 540.0f });
    m_frame.setFillColor(FRAME_FILL);
    m_frame.setOutlineColor(GOLD_COLOR);
    m_frame.setOutlineThickness(2.5f);
    m_frame.setPosition({ 50.0f, 30.0f });

    if (m_fontLoaded)
    {
        // Title
        m_titleText.setFont(m_font);
        m_titleText.setString("RECORDS & ACHIEVEMENTS");
        m_titleText.setCharacterSize(30);
        m_titleText.setFillColor(GOLD_COLOR);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.0f);
        UIUtils::centerOrigin(m_titleText);
        m_titleText.setPosition({ WIN_W / 2.0f, 65.0f });

        // Hint
        m_hintText.setFont(m_font);
        m_hintText.setString("L/R to flip pages   ENTER / SPACE / ESC TO RETURN");
        m_hintText.setCharacterSize(14);
        m_hintText.setFillColor(GRAY_COLOR);
        UIUtils::centerOrigin(m_hintText);
        m_hintText.setPosition({ WIN_W / 2.0f, 555.0f });

        // Stats header
        m_statsHeaderText.setFont(m_font);
        m_statsHeaderText.setString("CAREER STATISTICS");
        m_statsHeaderText.setCharacterSize(20);
        m_statsHeaderText.setFillColor(sf::Color(180, 220, 255));
        m_statsHeaderText.setOutlineColor(sf::Color::Black);
        m_statsHeaderText.setOutlineThickness(1.5f);
        m_statsHeaderText.setPosition({ 90.0f, 95.0f });

        // Stat rows
        struct StatDef { std::string key; std::string label; };
        static const std::vector<StatDef> statDefs = {
            {"enemies_killed", "ENEMIES STOMPED"},
            {"coins_collected", "COINS COLLECTED"},
            {"blocks_broken", "BLOCKS BROKEN"},
            {"total_deaths", "TOTAL DEATHS"},
            {"games_played", "GAMES PLAYED"},
            {"games_won", "GAMES WON"},
            {"total_score", "TOTAL SCORE"},
        };

        float statY = 125.0f;
        for (const auto& def : statDefs)
        {
            StatRow row;
            row.labelStr = def.label;

            int val = m_saveManager ? m_saveManager->getStat(def.key) : 0;
            if (def.key == "total_score")
            {
                std::string s = std::to_string(val);
                if (s.length() < 8) s = std::string(8 - s.length(), '0') + s;
                row.valueStr = s;
            }
            else
            {
                row.valueStr = std::to_string(val);
            }

            m_statRows.push_back(std::move(row));
            statY += 24.0f;
        }

        // Achievements header
        m_achieveHeaderText.setFont(m_font);
        m_achieveHeaderText.setString("ACHIEVEMENTS");
        m_achieveHeaderText.setCharacterSize(20);
        m_achieveHeaderText.setFillColor(sf::Color(255, 200, 80));
        m_achieveHeaderText.setOutlineColor(sf::Color::Black);
        m_achieveHeaderText.setOutlineThickness(1.5f);
        m_achieveHeaderText.setPosition({ 90.0f, 295.0f });

        setupAchievements();
    }
}

void StatsState::setupAchievements()
{
    if (!m_fontLoaded) return;

    // Single source of truth — AchievementDefs.h
    const auto& defs = getAchievementDefinitions();

    const float totalW = COLS * CARD_W + (COLS - 1) * GAP_X;
    const float startX = (WIN_W - totalW) / 2.0f;

    for (std::size_t i = 0; i < defs.size(); ++i)
    {
        const auto& def = defs[i];
        TrophyCard trophy;

        int col = static_cast<int>(i % COLS);
        int row = static_cast<int>(i / COLS);
        float x = startX + col * (CARD_W + GAP_X);
        float y = START_Y + row * (CARD_H + GAP_Y);

        trophy.unlocked = m_saveManager && m_saveManager->isAchievementUnlocked(def.id);

        trophy.card.setSize({ CARD_W, CARD_H });
        trophy.card.setPosition({ x, y });
        trophy.card.setFillColor(trophy.unlocked ? UNLOCKED_BG : LOCKED_BG);
        trophy.card.setOutlineThickness(2.0f);
        trophy.card.setOutlineColor(trophy.unlocked ? UNLOCKED_OUTLINE : LOCKED_OUTLINE);

        trophy.iconStr = def.icon;
        trophy.nameStr = def.displayName;
        trophy.descStr = def.description;

        m_trophies.push_back(std::move(trophy));
    }
}

void StatsState::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyEvent->code == sf::Keyboard::Key::Left)
        {
            if (m_currentPage > 0) --m_currentPage;
        }
        else if (keyEvent->code == sf::Keyboard::Key::Right)
        {
            int totalPages = (static_cast<int>(m_trophies.size()) + ACHIEVEMENTS_PER_PAGE - 1) / ACHIEVEMENTS_PER_PAGE;
            if (m_currentPage < totalPages - 1) ++m_currentPage;
        }
        else
        {
            returnToMenu();
        }
    }
    else if (event.is<sf::Event::MouseButtonPressed>())
    {
        returnToMenu();
    }
}

void StatsState::returnToMenu()
{
    if (auto* manager = getStateManager())
    {
        manager->changeState(std::make_unique<MenuState>(m_settings, m_saveManager, m_soundManager));
    }
}

void StatsState::update(float deltaTime)
{
    (void)deltaTime;
}

void StatsState::render(sf::RenderWindow& window) const
{
    sf::View defaultView({ WIN_W / 2.0f, WIN_H / 2.0f }, { WIN_W, WIN_H });
    window.setView(defaultView);

    window.draw(m_background);
    window.draw(m_frame);

    if (m_fontLoaded)
    {
        window.draw(m_titleText);

        // Stats section
        window.draw(m_statsHeaderText);
        float statY = 125.0f;
        for (const auto& row : m_statRows)
        {
            sf::Text label(m_font, row.labelStr, 16);
            label.setFillColor(WHITE_COLOR);
            label.setOutlineColor(sf::Color::Black);
            label.setOutlineThickness(1.0f);
            label.setPosition({ 90.0f, statY });
            window.draw(label);

            sf::Text value(m_font, row.valueStr, 16);
            value.setFillColor(GOLD_COLOR);
            value.setOutlineColor(sf::Color::Black);
            value.setOutlineThickness(1.0f);
            value.setPosition({ 500.0f, statY });
            window.draw(value);

            statY += 24.0f;
        }

        // Achievements section
        window.draw(m_achieveHeaderText);
        const float totalW = COLS * CARD_W + (COLS - 1) * GAP_X;
        const float startX = (WIN_W - totalW) / 2.0f;

        int totalPages = (static_cast<int>(m_trophies.size()) + ACHIEVEMENTS_PER_PAGE - 1) / ACHIEVEMENTS_PER_PAGE;
        int pageStart = m_currentPage * ACHIEVEMENTS_PER_PAGE;
        int pageEnd = std::min(pageStart + ACHIEVEMENTS_PER_PAGE, static_cast<int>(m_trophies.size()));

        for (int i = pageStart; i < pageEnd; ++i)
        {
            const auto& trophy = m_trophies[i];
            int localIndex = i - pageStart;
            int col = localIndex % COLS;
            int row = localIndex / COLS;
            float x = startX + col * (CARD_W + GAP_X);
            float y = START_Y + row * (CARD_H + GAP_Y);

            // Reposition card to page-relative position (setupAchievements uses global index)
            sf::RectangleShape card = trophy.card;
            card.setPosition({ x, y });
            window.draw(card);

            sf::Text icon(m_font, trophy.iconStr, 14);
            icon.setFillColor(trophy.unlocked ? GOLD_COLOR : GRAY_COLOR);
            icon.setPosition({ x + 10.0f, y + 8.0f });
            window.draw(icon);

            sf::Text name(m_font, trophy.nameStr, 12);
            name.setFillColor(trophy.unlocked ? GOLD_COLOR : GRAY_COLOR);
            name.setOutlineColor(sf::Color::Black);
            name.setOutlineThickness(1.0f);
            name.setPosition({ x + 10.0f, y + 28.0f });
            window.draw(name);

            sf::Text desc(m_font, trophy.descStr, 8);
            desc.setFillColor(trophy.unlocked ? WHITE_COLOR : sf::Color(100, 100, 110));
            desc.setPosition({ x + 10.0f, y + 50.0f });
            window.draw(desc);
        }

        // Page indicator
        if (totalPages > 1)
        {
            sf::Text pageLabel(m_font, "Page " + std::to_string(m_currentPage + 1) + " / " + std::to_string(totalPages), 14);
            pageLabel.setFillColor(GRAY_COLOR);
            UIUtils::centerOrigin(pageLabel);
            pageLabel.setPosition({ WIN_W / 2.0f, 530.0f });
            window.draw(pageLabel);
        }

        window.draw(m_hintText);
    }
}
