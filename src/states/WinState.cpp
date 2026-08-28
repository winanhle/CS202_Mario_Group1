#include "WinState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "InitialsEntryState.h"
#include "../ui/SaveManager.h"
#include "../core/StateManager.h"
#include "../interfaces/ISettingsManager.h"
#include <cmath>

static constexpr sf::Color WIN_BG_COLOR     = sf::Color(15, 20, 45);
static constexpr sf::Color FRAME_FILL       = sf::Color(25, 35, 70, 200);
static constexpr sf::Color FRAME_OUTLINE    = sf::Color(90, 120, 200);

static constexpr sf::Color CARD_IDLE_FILL   = sf::Color(30, 45, 80, 210);
static constexpr sf::Color CARD_SEL_FILL    = sf::Color(55, 85, 160, 235);
static constexpr sf::Color CARD_IDLE_OUTLINE= sf::Color(60, 80, 130);
static constexpr sf::Color CARD_SEL_OUTLINE = sf::Color(150, 190, 255);

static constexpr int LIVES_BONUS_MULTIPLIER = 1000;

WinState::WinState(std::shared_ptr<ISettingsManager> settings,
                   const GameConfig& config,
                   int baseScore,
                   int livesRemaining)
    : m_settings(std::move(settings))
    , m_config(config)
    , m_baseScore(baseScore)
    , m_livesRemaining(livesRemaining)
    , m_totalScore(baseScore + std::max(0, livesRemaining) * LIVES_BONUS_MULTIPLIER)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    // 1. Textures & Sprites
    m_peachLoaded = m_peachTexture.loadFromFile("assets/texture/hero/peach.png");
    if (m_peachLoaded)
    {
        m_peachSprite.setTexture(m_peachTexture);
        m_peachSprite.setTextureRect(sf::IntRect({1, 1}, {16, 24}));
        m_peachSprite.setScale({3.f, 3.f});
        m_peachSprite.setOrigin({8.f, 24.f});
    }

    std::string p1TexPath = (m_config.player1Character == CharacterType::Mario)
        ? "assets/texture/hero/mario.png"
        : "assets/texture/hero/luigi.png";

    m_heroLoaded = m_heroTexture.loadFromFile(p1TexPath);
    if (m_heroLoaded)
    {
        m_heroSprite.setTexture(m_heroTexture);
        if (m_config.player1Character == CharacterType::Mario)
        {
            m_heroSprite.setTextureRect(sf::IntRect({1, 6}, {16, 18}));
            m_heroSprite.setOrigin({8.f, 18.f});
        }
        else
        {
            m_heroSprite.setTextureRect(sf::IntRect({1, 2}, {16, 23}));
            m_heroSprite.setOrigin({8.f, 23.f});
        }
        m_heroSprite.setScale({3.f, 3.f});
    }

    if (m_config.mode == GameMode::TwoPlayer)
    {
        std::string p2TexPath = (m_config.player2Character == CharacterType::Mario)
            ? "assets/texture/hero/mario.png"
            : "assets/texture/hero/luigi.png";
        m_hero2Loaded = m_hero2Texture.loadFromFile(p2TexPath);
        if (m_hero2Loaded)
        {
            m_hero2Sprite.setTexture(m_hero2Texture);
            if (m_config.player2Character == CharacterType::Mario)
            {
                m_hero2Sprite.setTextureRect(sf::IntRect({1, 6}, {16, 18}));
                m_hero2Sprite.setOrigin({8.f, 18.f});
            }
            else
            {
                m_hero2Sprite.setTextureRect(sf::IntRect({1, 2}, {16, 23}));
                m_hero2Sprite.setOrigin({8.f, 23.f});
            }
            m_hero2Sprite.setScale({-3.f, 3.f}); // Face left toward Peach
        }
    }

    // 2. UI Text Setup
    if (m_fontLoaded)
    {
        // Title
        m_titleText.setFont(m_font);
        m_titleText.setString("CONGRATULATIONS!");
        m_titleText.setCharacterSize(38);
        m_titleText.setFillColor(sf::Color(255, 220, 50));
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(3.0f);
        centerOrigin(m_titleText);
        m_titleText.setPosition({ WIN_W / 2.0f, 60.0f });

        // Subtitle
        std::string subString;
        if (m_config.mode == GameMode::TwoPlayer)
            subString = "PEACH IS RESCUED! THANK YOU HEROES!";
        else if (m_config.player1Character == CharacterType::Mario)
            subString = "PEACH IS RESCUED! THANK YOU MARIO!";
        else
            subString = "PEACH IS RESCUED! THANK YOU LUIGI!";

        m_subtitleText.setFont(m_font);
        m_subtitleText.setString(subString);
        m_subtitleText.setCharacterSize(16);
        m_subtitleText.setFillColor(sf::Color(255, 210, 230));
        centerOrigin(m_subtitleText);
        m_subtitleText.setPosition({ WIN_W / 2.0f, 105.0f });

        // Score Breakdown
        m_scoreLabelText.setFont(m_font);
        m_scoreLabelText.setString("BASE SCORE: " + std::to_string(m_baseScore));
        m_scoreLabelText.setCharacterSize(15);
        m_scoreLabelText.setFillColor(sf::Color(200, 220, 255));
        centerOrigin(m_scoreLabelText);
        m_scoreLabelText.setPosition({ WIN_W / 2.0f, 240.0f });

        m_livesBonusText.setFont(m_font);
        m_livesBonusText.setString("LIVES BONUS (" + std::to_string(m_livesRemaining) + " x " +
                                   std::to_string(LIVES_BONUS_MULTIPLIER) + "): +" +
                                   std::to_string(std::max(0, m_livesRemaining) * LIVES_BONUS_MULTIPLIER));
        m_livesBonusText.setCharacterSize(15);
        m_livesBonusText.setFillColor(sf::Color(180, 240, 180));
        centerOrigin(m_livesBonusText);
        m_livesBonusText.setPosition({ WIN_W / 2.0f, 268.0f });

        m_totalScoreText.setFont(m_font);
        m_totalScoreText.setString("FINAL SCORE: " + std::to_string(m_totalScore));
        m_totalScoreText.setCharacterSize(22);
        m_totalScoreText.setFillColor(sf::Color(255, 235, 80));
        m_totalScoreText.setOutlineColor(sf::Color::Black);
        m_totalScoreText.setOutlineThickness(2.0f);
        centerOrigin(m_totalScoreText);
        m_totalScoreText.setPosition({ WIN_W / 2.0f, 305.0f });

        // Hint Text
        m_hintText.setFont(m_font);
        m_hintText.setString("W/S or Up/Down/Mouse to navigate   Enter / Click to select");
        m_hintText.setCharacterSize(13);
        m_hintText.setFillColor(sf::Color(160, 170, 200));
        centerOrigin(m_hintText);
        m_hintText.setPosition({ WIN_W / 2.0f, 540.0f });
    }

    // 3. Option Cards
    const float CARD_W = 280.0f;
    const float CARD_H = 48.0f;
    const float START_Y = 370.0f;
    const float SPACING = 65.0f;

    for (int i = 0; i < OPTION_COUNT; ++i)
    {
        m_optionCards[i].setSize({ CARD_W, CARD_H });
        m_optionCards[i].setOrigin({ CARD_W / 2.0f, CARD_H / 2.0f });
        m_optionCards[i].setPosition({ WIN_W / 2.0f, START_Y + i * SPACING });
        m_optionCards[i].setOutlineThickness(2.5f);

        if (m_fontLoaded)
        {
            sf::Text& optText = getOptionText(i);
            optText.setFont(m_font);
            optText.setCharacterSize(18);
            optText.setPosition({ WIN_W / 2.0f, START_Y + i * SPACING });
        }
    }

    // 4. UINavigator Setup
    m_nav.setAxis(UINavigator::Axis::Vertical);
    m_nav.getHitbox = [this](int i) { return m_optionCards[i].getGlobalBounds(); };
    m_nav.onActivate = [this](int i) { confirmSelection(); };
    m_nav.onSelectionChanged = [this](int, int) { refreshUI(); };

    // 5. Background shapes (pre-built, drawn each frame without reconstruction)
    m_background.setFillColor(WIN_BG_COLOR);
    m_frame.setPosition({ 30.0f, 30.0f });
    m_frame.setFillColor(FRAME_FILL);
    m_frame.setOutlineThickness(2.5f);
    m_frame.setOutlineColor(FRAME_OUTLINE);

    refreshUI();
}

void WinState::refreshUI()
{
    const std::array<std::string, OPTION_COUNT> baseLabels = {
        "PLAY AGAIN",
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
            optText.setFillColor(selected ? sf::Color(255, 240, 100) : sf::Color(220, 230, 255));

            centerOrigin(optText);
        }
    }
}

void WinState::handleInput(const sf::Event& event)
{
    if (const auto* resizeEvent = event.getIf<sf::Event::Resized>())
    {
        m_windowSize = resizeEvent->size;
    }

    m_nav.handleInput(event, m_windowSize);
}

void WinState::confirmSelection()
{
    if (m_nav.getSelectedIndex() == 0)
    {
        playAgain();
    }
    else
    {
        returnToMenu();
    }
}

void WinState::playAgain()
{
    auto* manager = getStateManager();
    if (manager)
    {
        manager->changeState(std::make_unique<PlayState>(m_config, m_settings, false));
    }
}

void WinState::returnToMenu()
{
    auto* manager = getStateManager();
    if (manager)
    {
        if (SaveManager::checkIsHighScore(m_totalScore))
        {
            manager->changeState(std::make_unique<InitialsEntryState>(m_totalScore, m_settings, m_config));
        }
        else
        {
            manager->changeState(std::make_unique<MenuState>(m_settings));
        }
    }
}

void WinState::update(float deltaTime)
{
    m_animTimer += deltaTime;

    // Gentle bounce animation for character sprites
    float bounce    = std::sin(m_animTimer * 4.0f) * 6.0f;
    float baselineY = 210.0f;
    float peachY    = baselineY + bounce;
    float heroY     = baselineY - bounce; // Counter-bounce for playful interaction

    if (m_config.mode == GameMode::TwoPlayer)
    {
        if (m_heroLoaded)   m_heroSprite.setPosition({ WIN_W / 2.0f - 85.0f, heroY });
        if (m_peachLoaded)  m_peachSprite.setPosition({ WIN_W / 2.0f, peachY });
        if (m_hero2Loaded)  m_hero2Sprite.setPosition({ WIN_W / 2.0f + 85.0f, heroY });
    }
    else
    {
        if (m_heroLoaded)   m_heroSprite.setPosition({ WIN_W / 2.0f - 45.0f, heroY });
        if (m_peachLoaded)  m_peachSprite.setPosition({ WIN_W / 2.0f + 45.0f, peachY });
    }

    // Scale pulse on selected card
    if (m_fontLoaded)
    {
        float pulse = 1.0f + 0.03f * std::sin(m_animTimer * 5.0f);
        m_optionCards[m_nav.getSelectedIndex()].setScale({ pulse, pulse });
        getOptionText(m_nav.getSelectedIndex()).setScale({ pulse, pulse });

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

void WinState::render(sf::RenderWindow& window) const
{
    // Standard 800x600 view
    sf::View defaultView({ WIN_W / 2.0f, WIN_H / 2.0f }, { WIN_W, WIN_H });
    window.setView(defaultView);

    // Background
    window.draw(m_background);

    // Decorative inner frame
    window.draw(m_frame);

    // Texts
    window.draw(m_titleText);
    window.draw(m_subtitleText);

    // Sprites
    if (m_heroLoaded)  window.draw(m_heroSprite);
    if (m_peachLoaded) window.draw(m_peachSprite);
    if (m_config.mode == GameMode::TwoPlayer && m_hero2Loaded)
        window.draw(m_hero2Sprite);

    window.draw(m_scoreLabelText);
    window.draw(m_livesBonusText);
    window.draw(m_totalScoreText);

    // Option cards
    for (int i = 0; i < OPTION_COUNT; ++i)
    {
        window.draw(m_optionCards[i]);
        window.draw(getOptionText(i));
    }

    window.draw(m_hintText);
}
