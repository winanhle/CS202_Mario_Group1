#include "GameOverState.h"
#include "MenuState.h"
#include "PlayState.h"
#include "InitialsEntryState.h"
#include "../interfaces/ISaveManager.h"
#include "../core/StateManager.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace
{
    constexpr sf::Color BG_COLOR{18, 18, 24};
    constexpr sf::Color CARD_IDLE_FILL{35, 35, 45, 200};
    constexpr sf::Color CARD_SEL_FILL{160, 30, 30, 230};
    constexpr sf::Color CARD_IDLE_OUTLINE{70, 70, 85};
    constexpr sf::Color CARD_SEL_OUTLINE{255, 215, 0};
}

GameOverState::GameOverState(std::shared_ptr<ISettingsManager> settings,
                             std::shared_ptr<ISaveManager> saveManager,
                             const GameConfig& config)
    : m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_config(config)
    , m_fontLoaded(false)
    , m_animTimer(0.0f)
    , m_finalScore(0)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    if (m_fontLoaded)
    {
        // 1. Title
        m_titleText.setFont(m_font);
        m_titleText.setString("GAME OVER");
        m_titleText.setCharacterSize(64);
        m_titleText.setFillColor(sf::Color(230, 40, 40));
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(3.0f);

        centerOrigin(m_titleText);
        m_titleText.setPosition({ WIN_W / 2.0f, 130.0f });

        // 2. Score
        m_scoreText.setFont(m_font);
        m_scoreText.setString("FINAL SCORE: 0");
        m_scoreText.setCharacterSize(26);
        m_scoreText.setFillColor(sf::Color(255, 215, 0));
        m_scoreText.setOutlineColor(sf::Color::Black);
        m_scoreText.setOutlineThickness(1.5f);

        centerOrigin(m_scoreText);
        m_scoreText.setPosition({ WIN_W / 2.0f, 220.0f });

        // 3. Option buttons
        const std::array<std::string, OPTION_COUNT> optionLabels = {
            "RETRY",
            "MAIN MENU"
        };

        const float startY = 320.0f;
        const float gapY = 80.0f;
        const sf::Vector2f cardSize(280.0f, 52.0f);

        for (int i = 0; i < OPTION_COUNT; ++i)
        {
            float y = startY + static_cast<float>(i) * gapY;

            // Card background
            m_optionCards[i].setSize(cardSize);
            m_optionCards[i].setOrigin({ cardSize.x / 2.0f, cardSize.y / 2.0f });
            m_optionCards[i].setPosition({ WIN_W / 2.0f, y });
            m_optionCards[i].setOutlineThickness(2.5f);

            // Card text
            sf::Text& optText = getOptionText(i);
            optText.setFont(m_font);
            optText.setString(optionLabels[i]);
            optText.setCharacterSize(24);
            optText.setOutlineColor(sf::Color::Black);
            optText.setOutlineThickness(1.5f);

            centerOrigin(optText);
            optText.setPosition({ WIN_W / 2.0f, y });
        }

        // 4. Bottom hint
        m_hintText.setFont(m_font);
        m_hintText.setString("W/S or Up/Down to navigate   Enter / Click to select");
        m_hintText.setCharacterSize(14);
        m_hintText.setFillColor(sf::Color(160, 160, 170));

        centerOrigin(m_hintText);
        m_hintText.setPosition({ WIN_W / 2.0f, 530.0f });
    }

    m_nav.getHitbox = [this](int i) { return m_optionCards[i].getGlobalBounds(); };
    m_nav.onActivate = [this](int i) { confirmSelection(); };
    m_nav.onSelectionChanged = [this](int oldIdx, int newIdx) { refreshUI(); };

    refreshUI();
}

void GameOverState::setFinalScore(int score)
{
    m_finalScore = score;
    if (m_fontLoaded)
    {
        m_scoreText.setString("FINAL SCORE: " + std::to_string(m_finalScore));
        centerOrigin(m_scoreText);
        m_scoreText.setPosition({ WIN_W / 2.0f, 220.0f });
    }
}

void GameOverState::refreshUI()
{
    const std::array<std::string, OPTION_COUNT> baseLabels = {
        "RETRY",
        "MAIN MENU"
    };

    for (int i = 0; i < OPTION_COUNT; ++i)
    {
        bool selected = (i == m_nav.getSelectedIndex());
        m_optionCards[i].setFillColor(selected ? CARD_SEL_FILL : CARD_IDLE_FILL);
        m_optionCards[i].setOutlineColor(selected ? CARD_SEL_OUTLINE : CARD_IDLE_OUTLINE);

        if (m_fontLoaded)
        {
            sf::Text& optText = getOptionText(i);
            optText.setString(selected ? "> " + baseLabels[i] + " <" : baseLabels[i]);
            optText.setFillColor(selected ? sf::Color(255, 240, 100) : sf::Color(210, 210, 220));

            centerOrigin(optText);
        }
    }
}

void GameOverState::handleInput(const sf::Event& event)
{
    if (const auto* resizeEvent = event.getIf<sf::Event::Resized>())
    {
        m_windowSize = resizeEvent->size;
    }

    m_nav.handleInput(event, m_windowSize);
}

void GameOverState::confirmSelection()
{
    if (m_nav.getSelectedIndex() == 0)
    {
        retryGame();
    }
    else
    {
        returnToMenu();
    }
}

void GameOverState::retryGame()
{
    auto* manager = getStateManager();
    if (manager)
    {
        manager->changeState(std::make_unique<PlayState>(m_config, m_settings, m_saveManager, false));
    }
}

void GameOverState::returnToMenu()
{
    auto* manager = getStateManager();
    if (manager)
    {
        if (m_saveManager && m_saveManager->isHighScore(m_finalScore))
        {
            manager->changeState(std::make_unique<InitialsEntryState>(m_finalScore, m_settings, m_saveManager, m_config));
        }
        else
        {
            manager->changeState(std::make_unique<MenuState>(m_settings, m_saveManager));
        }
    }
}

void GameOverState::update(float deltaTime)
{
    m_animTimer += deltaTime;

    // Subtle scale pulsing for the selected card
    if (m_fontLoaded)
    {
        float pulse = 1.0f + 0.03f * std::sin(m_animTimer * 5.0f);
        m_optionCards[m_nav.getSelectedIndex()].setScale({ pulse, pulse });
        getOptionText(m_nav.getSelectedIndex()).setScale({ pulse, pulse });

        // Reset non-selected cards
        for (int i = 0; i < OPTION_COUNT; ++i)
        {
            if (i != m_nav.getSelectedIndex())
            {
                m_optionCards[i].setScale({ 1.0f, 1.0f });
                getOptionText(i).setScale({ 1.0f, 1.0f });
            }
        }
    }
}

void GameOverState::render(sf::RenderWindow& window) const
{
    // Ensure render uses standard 800x600 view
    sf::View defaultView({ WIN_W / 2.0f, WIN_H / 2.0f }, { WIN_W, WIN_H });
    window.setView(defaultView);

    // Dark stylish background
    sf::RectangleShape background({ WIN_W, WIN_H });
    background.setFillColor(BG_COLOR);
    window.draw(background);

    // Decorative inner box frame
    sf::RectangleShape frame({ WIN_W - 80.0f, WIN_H - 80.0f });
    frame.setPosition({ 40.0f, 40.0f });
    frame.setFillColor(sf::Color(25, 25, 35, 180));
    frame.setOutlineThickness(2.0f);
    frame.setOutlineColor(sf::Color(60, 60, 80));
    window.draw(frame);

    window.draw(m_titleText);
    window.draw(m_scoreText);

    for (int i = 0; i < OPTION_COUNT; ++i)
    {
        window.draw(m_optionCards[i]);
        window.draw(getOptionText(i));
    }

    window.draw(m_hintText);
}
