#include "InitialsEntryState.h"
#include "LeaderboardState.h"
#include "../core/StateManager.h"
#include "../interfaces/ISaveManager.h"
#include "../interfaces/ISoundManager.h"
#include <algorithm>
#include <iostream>

static constexpr sf::Color GOLD_COLOR{ 255, 215, 0 };
static constexpr sf::Color WHITE_COLOR{ 255, 255, 255 };
static constexpr sf::Color GRAY_COLOR{ 160, 160, 160 };
static constexpr sf::Color BOX_ACTIVE_COLOR{ 50, 50, 90, 230 };
static constexpr sf::Color BOX_INACTIVE_COLOR{ 25, 25, 25, 200 };
static constexpr sf::Color OUTLINE_ACTIVE_COLOR{ 255, 215, 0 };
static constexpr sf::Color OUTLINE_INACTIVE_COLOR{ 80, 80, 80 };

InitialsEntryState::InitialsEntryState(
    int finalScore,
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    const GameConfig& config,
    std::shared_ptr<ISoundManager> soundManager
)
    : m_score(finalScore)
    , m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_soundManager(std::move(soundManager))
    , m_config(config)
    , m_slots{
        SlotVisual{ sf::RectangleShape(), sf::Text(m_font) },
        SlotVisual{ sf::RectangleShape(), sf::Text(m_font) },
        SlotVisual{ sf::RectangleShape(), sf::Text(m_font) },
        SlotVisual{ sf::RectangleShape(), sf::Text(m_font) },
        SlotVisual{ sf::RectangleShape(), sf::Text(m_font) }
    }
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    if (!m_fontLoaded)
    {
        std::cerr << "[InitialsEntryState] ERROR: Failed to open font assets/fonts/SuperMario256.ttf\n";
    }

    m_background.setSize({ 800.0f, 600.0f });
    m_background.setFillColor(sf::Color(10, 10, 20, 240));

    m_frame.setSize({ 640.0f, 440.0f });
    m_frame.setFillColor(sf::Color(0, 0, 0, 180));
    m_frame.setOutlineColor(GOLD_COLOR);
    m_frame.setOutlineThickness(3.0f);
    m_frame.setPosition({ 80.0f, 80.0f });

    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("NEW HIGH SCORE!");
        m_titleText.setCharacterSize(36);
        m_titleText.setFillColor(GOLD_COLOR);
        centerOrigin(m_titleText);
        m_titleText.setPosition({ 400.0f, 130.0f });

        m_scoreText.setFont(m_font);
        m_scoreText.setString("SCORE: " + std::to_string(m_score));
        m_scoreText.setCharacterSize(26);
        m_scoreText.setFillColor(WHITE_COLOR);
        centerOrigin(m_scoreText);
        m_scoreText.setPosition({ 400.0f, 185.0f });

        m_promptText.setFont(m_font);
        m_promptText.setString("TYPE YOUR INITIALS (5 LETTERS)");
        m_promptText.setCharacterSize(20);
        m_promptText.setFillColor(GRAY_COLOR);
        centerOrigin(m_promptText);
        m_promptText.setPosition({ 400.0f, 235.0f });

        m_hintText.setFont(m_font);
        m_hintText.setString("KEYBOARD: TYPE LETTERS | BACKSPACE: DELETE | ENTER: CONFIRM");
        m_hintText.setCharacterSize(14);
        m_hintText.setFillColor(GRAY_COLOR);
        centerOrigin(m_hintText);
        m_hintText.setPosition({ 400.0f, 475.0f });

        const float startX = 220.0f;
        const float gapX = 90.0f;
        const float slotY = 330.0f;

        for (int i = 0; i < 5; ++i)
        {
            float posX = startX + static_cast<float>(i) * gapX;

            m_slots[static_cast<std::size_t>(i)].box.setSize({ 70.0f, 85.0f });
            m_slots[static_cast<std::size_t>(i)].box.setOrigin({ 35.0f, 42.5f });
            m_slots[static_cast<std::size_t>(i)].box.setPosition({ posX, slotY });

            m_slots[static_cast<std::size_t>(i)].text.setFont(m_font);
            m_slots[static_cast<std::size_t>(i)].text.setString(std::string(1, m_letters[static_cast<std::size_t>(i)]));
            m_slots[static_cast<std::size_t>(i)].text.setCharacterSize(38);
            centerOrigin(m_slots[static_cast<std::size_t>(i)].text);
            m_slots[static_cast<std::size_t>(i)].text.setPosition({ posX, slotY - 4.0f });
        }
    }
}

void InitialsEntryState::handleInput(const sf::Event& event)
{
    // 1. Direct text typing from keyboard
    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>())
    {
        char32_t unicode = textEntered->unicode;
        if (unicode >= 'a' && unicode <= 'z')
        {
            unicode = unicode - 'a' + 'A';
        }

        if ((unicode >= 'A' && unicode <= 'Z') || (unicode >= '0' && unicode <= '9'))
        {
            char c = static_cast<char>(unicode);
            m_letters[static_cast<std::size_t>(m_activeSlot)] = c;
            m_slots[static_cast<std::size_t>(m_activeSlot)].text.setString(std::string(1, c));
            centerOrigin(m_slots[static_cast<std::size_t>(m_activeSlot)].text);

            if (m_activeSlot < 4)
            {
                ++m_activeSlot;
            }
            return;
        }
    }

    // 2. Control keys (Backspace, Enter, Arrows)
    if (const auto* keyPress = event.getIf<sf::Event::KeyPressed>())
    {
        switch (keyPress->code)
        {
        case sf::Keyboard::Key::Backspace:
        {
            if (m_letters[static_cast<std::size_t>(m_activeSlot)] != '_')
            {
                m_letters[static_cast<std::size_t>(m_activeSlot)] = '_';
                m_slots[static_cast<std::size_t>(m_activeSlot)].text.setString("_");
                centerOrigin(m_slots[static_cast<std::size_t>(m_activeSlot)].text);
            }
            else if (m_activeSlot > 0)
            {
                --m_activeSlot;
                m_letters[static_cast<std::size_t>(m_activeSlot)] = '_';
                m_slots[static_cast<std::size_t>(m_activeSlot)].text.setString("_");
                centerOrigin(m_slots[static_cast<std::size_t>(m_activeSlot)].text);
            }
            break;
        }
        case sf::Keyboard::Key::Enter:
            submitScore();
            break;
        case sf::Keyboard::Key::Left:
            if (m_activeSlot > 0)
                --m_activeSlot;
            break;
        case sf::Keyboard::Key::Right:
        case sf::Keyboard::Key::Tab:
        case sf::Keyboard::Key::Space:
            if (m_activeSlot < 4)
                ++m_activeSlot;
            break;
        default:
            break;
        }
    }
}

void InitialsEntryState::submitScore()
{
    std::string initials;
    for (int i = 0; i < 5; ++i)
    {
        char c = m_letters[static_cast<std::size_t>(i)];
        if (c == '_' || c == ' ')
            initials += 'A';
        else
            initials += c;
    }

    if (m_saveManager)
    {
        m_saveManager->addHighScore(initials, m_score);
    }

    if (auto* manager = getStateManager())
    {
        manager->changeState(std::make_unique<LeaderboardState>(m_settings, m_saveManager, m_soundManager, m_config));
    }
}

void InitialsEntryState::update(float deltaTime)
{
    m_cursorBlinkTimer += deltaTime;
    if (m_cursorBlinkTimer >= 0.35f)
    {
        m_cursorBlinkTimer = 0.0f;
        m_showCursor = !m_showCursor;
    }

    updateLayout();
}

void InitialsEntryState::updateLayout()
{
    for (int i = 0; i < 5; ++i)
    {
        auto idx = static_cast<std::size_t>(i);
        bool isActive = (i == m_activeSlot);

        if (isActive)
        {
            m_slots[idx].box.setFillColor(BOX_ACTIVE_COLOR);
            m_slots[idx].box.setOutlineColor(m_showCursor ? OUTLINE_ACTIVE_COLOR : sf::Color::Transparent);
            m_slots[idx].box.setOutlineThickness(2.5f);
            m_slots[idx].text.setFillColor(GOLD_COLOR);
        }
        else
        {
            m_slots[idx].box.setFillColor(BOX_INACTIVE_COLOR);
            m_slots[idx].box.setOutlineColor(OUTLINE_INACTIVE_COLOR);
            m_slots[idx].box.setOutlineThickness(1.5f);
            m_slots[idx].text.setFillColor(WHITE_COLOR);
        }
    }
}

void InitialsEntryState::render(sf::RenderWindow& window) const
{
    if (!m_fontLoaded)
        return;

    window.draw(m_background);
    window.draw(m_frame);
    window.draw(m_titleText);
    window.draw(m_scoreText);
    window.draw(m_promptText);

    for (std::size_t i = 0; i < 5; ++i)
    {
        window.draw(m_slots[i].box);
        window.draw(m_slots[i].text);
    }

    window.draw(m_hintText);
}
