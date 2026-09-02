#include "../ui/UIUtils.h"
#include "WinState.h"
#include "PlayState.h"
#include "MenuState.h"
#include "InitialsEntryState.h"

#include "../interfaces/ISaveManager.h"
#include "../interfaces/ISoundManager.h"
#include "../interfaces/ISettingsManager.h"
#include "../core/StateManager.h"

#include <cmath>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>

static constexpr sf::Color WIN_BG_COLOR      = sf::Color(15, 20, 45);
static constexpr sf::Color FRAME_FILL        = sf::Color(25, 35, 70, 200);
static constexpr sf::Color FRAME_OUTLINE     = sf::Color(90, 120, 200);

static constexpr sf::Color CARD_IDLE_FILL    = sf::Color(30, 45, 80, 210);
static constexpr sf::Color CARD_SEL_FILL     = sf::Color(55, 85, 160, 235);
static constexpr sf::Color CARD_IDLE_OUTLINE = sf::Color(60, 80, 130);
static constexpr sf::Color CARD_SEL_OUTLINE  = sf::Color(150, 190, 255);

static constexpr int LIVES_BONUS_MULTIPLIER = 1000;

WinState::WinState(
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    const GameConfig& config,
    int baseScore,
    int livesRemaining,
    std::shared_ptr<ISoundManager> soundManager
)
    : m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_soundManager(std::move(soundManager))
    , m_config(config)
    , m_baseScore(baseScore)
    , m_livesRemaining(livesRemaining)
    , m_totalScore(
        baseScore +
        std::max(0, livesRemaining) * LIVES_BONUS_MULTIPLIER
    )
{


    // ==================== FONT ====================
    m_fontLoaded =
        m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    // ==================== PEACH ====================
    m_peachLoaded =
        m_peachTexture.loadFromFile(
            "assets/texture/hero/peach.png"
        );

    if (m_peachLoaded)
    {
        m_peachSprite.setTexture(m_peachTexture);
        m_peachSprite.setTextureRect(
            sf::IntRect({1, 1}, {16, 24})
        );
        m_peachSprite.setScale({3.f, 3.f});
        m_peachSprite.setOrigin({8.f, 24.f});
    }

    // ==================== PLAYER 1 ====================
    std::string p1TexPath =
        (m_config.player1Character == CharacterType::Mario)
        ? "assets/texture/hero/mario.png"
        : "assets/texture/hero/luigi.png";

    m_heroLoaded =
        m_heroTexture.loadFromFile(p1TexPath);

    if (m_heroLoaded)
    {
        m_heroSprite.setTexture(m_heroTexture);

        if (m_config.player1Character == CharacterType::Mario)
        {
            m_heroSprite.setTextureRect(
                sf::IntRect({1, 6}, {16, 18})
            );
            m_heroSprite.setOrigin({8.f, 18.f});
        }
        else
        {
            m_heroSprite.setTextureRect(
                sf::IntRect({1, 2}, {16, 23})
            );
            m_heroSprite.setOrigin({8.f, 23.f});
        }

        m_heroSprite.setScale({3.f, 3.f});
    }

    // ==================== PLAYER 2 ====================
    if (m_config.mode == GameMode::TwoPlayer)
    {
        std::string p2TexPath =
            (m_config.player2Character == CharacterType::Mario)
            ? "assets/texture/hero/mario.png"
            : "assets/texture/hero/luigi.png";

        m_hero2Loaded =
            m_hero2Texture.loadFromFile(p2TexPath);

        if (m_hero2Loaded)
        {
            m_hero2Sprite.setTexture(m_hero2Texture);

            if (m_config.player2Character == CharacterType::Mario)
            {
                m_hero2Sprite.setTextureRect(
                    sf::IntRect({1, 6}, {16, 18})
                );
                m_hero2Sprite.setOrigin({8.f, 18.f});
            }
            else
            {
                m_hero2Sprite.setTextureRect(
                    sf::IntRect({1, 2}, {16, 23})
                );
                m_hero2Sprite.setOrigin({8.f, 23.f});
            }

            // Face toward Peach
            m_hero2Sprite.setScale({-3.f, 3.f});
        }
    }

    // ==================== TEXT ====================
    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("CONGRATULATIONS!");
        m_titleText.setCharacterSize(38);
        m_titleText.setFillColor(sf::Color(255, 220, 50));
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(3.0f);
        UIUtils::centerOrigin(m_titleText);
        m_titleText.setPosition({ WIN_W / 2.0f, 52.0f });

        std::string subtitle;
        if (m_config.mode == GameMode::TwoPlayer)
        {
            subtitle = "PEACH IS RESCUED! THANK YOU HEROES!";
        }
        else if (m_config.player1Character == CharacterType::Mario)
        {
            subtitle = "PEACH IS RESCUED! THANK YOU MARIO!";
        }
        else
        {
            subtitle = "PEACH IS RESCUED! THANK YOU LUIGI!";
        }

        m_subtitleText.setFont(m_font);
        m_subtitleText.setString(subtitle);
        m_subtitleText.setCharacterSize(16);
        m_subtitleText.setFillColor(sf::Color(255, 210, 230));
        UIUtils::centerOrigin(m_subtitleText);
        m_subtitleText.setPosition({ WIN_W / 2.0f, 92.0f });

        // Base score
        m_scoreLabelText.setFont(m_font);
        m_scoreLabelText.setString(
            "BASE SCORE: " +
            std::to_string(m_baseScore)
        );
        m_scoreLabelText.setCharacterSize(15);
        m_scoreLabelText.setFillColor(sf::Color(200, 220, 255));
        UIUtils::centerOrigin(m_scoreLabelText);
        m_scoreLabelText.setPosition({ WIN_W / 2.0f, 245.0f });

        int livesBonus = std::max(0, m_livesRemaining) * LIVES_BONUS_MULTIPLIER;
        m_livesBonusText.setFont(m_font);
        m_livesBonusText.setString(
            "LIVES BONUS (" +
            std::to_string(m_livesRemaining) +
            " x " +
            std::to_string(LIVES_BONUS_MULTIPLIER) +
            "): +" +
            std::to_string(livesBonus)
        );
        m_livesBonusText.setCharacterSize(15);
        m_livesBonusText.setFillColor(sf::Color(180, 240, 180));
        UIUtils::centerOrigin(m_livesBonusText);
        m_livesBonusText.setPosition({ WIN_W / 2.0f, 273.0f });

        // Final score
        m_totalScoreText.setFont(m_font);
        m_totalScoreText.setString(
            "FINAL SCORE: " +
            std::to_string(m_totalScore)
        );
        m_totalScoreText.setCharacterSize(22);
        m_totalScoreText.setFillColor(sf::Color(255, 235, 80));
        m_totalScoreText.setOutlineColor(sf::Color::Black);
        m_totalScoreText.setOutlineThickness(2.0f);
        UIUtils::centerOrigin(m_totalScoreText);
        m_totalScoreText.setPosition({ WIN_W / 2.0f, 310.0f });

        // Hint
        m_hintText.setFont(m_font);
        m_hintText.setString(
            "W/S or Up/Down/Mouse to navigate   "
            "Enter / Click to select"
        );
        m_hintText.setCharacterSize(13);
        m_hintText.setFillColor(sf::Color(160, 170, 200));
        UIUtils::centerOrigin(m_hintText);
        m_hintText.setPosition({ WIN_W / 2.0f, 540.0f });

        // Speech bubble for Princess Peach
        m_bubbleText.setFont(m_font);
        m_bubbleText.setString("Why'd you take so long!!!");
        m_bubbleText.setCharacterSize(12);
        m_bubbleText.setFillColor(sf::Color(200, 20, 50));
        UIUtils::centerOrigin(m_bubbleText);
    }

    // ==================== SPEECH BUBBLE SHAPES ====================
    m_bubbleBox.setSize({ 210.f, 32.f });
    m_bubbleBox.setFillColor(sf::Color(255, 255, 255));
    m_bubbleBox.setOutlineColor(sf::Color(20, 25, 45));
    m_bubbleBox.setOutlineThickness(2.5f);
    m_bubbleBox.setOrigin({ 105.f, 16.f });

    m_bubbleTail.setPointCount(3);
    m_bubbleTail.setPoint(0, { -105.f, -6.f });
    m_bubbleTail.setPoint(1, { -105.f, 6.f });
    m_bubbleTail.setPoint(2, { -128.f, 0.f }); // Pointer directed to Peach's face
    m_bubbleTail.setFillColor(sf::Color(255, 255, 255));
    m_bubbleTail.setOutlineColor(sf::Color(20, 25, 45));
    m_bubbleTail.setOutlineThickness(2.5f);

    // ==================== OPTION CARDS ====================
    const float CARD_W = 280.f;
    const float CARD_H = 48.f;
    const float START_Y = 370.f;
    const float SPACING = 65.f;

    for (int i = 0; i < OPTION_COUNT; ++i)
    {
        m_optionCards[i].setSize(
            {CARD_W, CARD_H}
        );

        m_optionCards[i].setOrigin(
            {CARD_W / 2.f, CARD_H / 2.f}
        );

        m_optionCards[i].setPosition(
            {WIN_W / 2.f, START_Y + i * SPACING}
        );

        m_optionCards[i].setOutlineThickness(2.5f);

        if (m_fontLoaded)
        {
            sf::Text& text = getOptionText(i);

            text.setFont(m_font);
            text.setCharacterSize(18);
            text.setPosition(
                {WIN_W / 2.f, START_Y + i * SPACING}
            );
        }
    }

    // ==================== UI NAVIGATOR ====================
    m_nav.setAxis(UINavigator::Axis::Vertical);

    m_nav.getHitbox =
        [this](int i)
        {
            return m_optionCards[i].getGlobalBounds();
        };

    m_nav.onActivate =
        [this](int)
        {
            confirmSelection();
        };

    m_nav.onSelectionChanged =
        [this](int, int)
        {
            if (m_soundManager)
                m_soundManager->playSelect();
            refreshUI();
        };

    // ==================== BACKGROUND ====================
    m_background.setFillColor(WIN_BG_COLOR);

    m_frame.setPosition({30.f, 30.f});
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

            UIUtils::centerOrigin(optText);
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
    if (m_soundManager)
        m_soundManager->playStomp();

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
        manager->changeState(std::make_unique<PlayState>(m_config, m_settings, m_saveManager, false, m_soundManager));
    }
}

void WinState::returnToMenu()
{
    // Record career stats on victory
    if (m_saveManager)
    {
        m_saveManager->recordStat("games_played", 1);
        m_saveManager->recordStat("total_score", m_totalScore);
        m_saveManager->evaluateAchievements();
        m_saveManager->flushStats();
    }

    auto* manager = getStateManager();
    if (manager)
    {
        if (m_saveManager && m_saveManager->isHighScore(m_totalScore))
        {
            manager->changeState(std::make_unique<InitialsEntryState>(m_totalScore, m_settings, m_saveManager, m_config, m_soundManager));
        }
        else
        {
            manager->changeState(std::make_unique<MenuState>(m_settings, m_saveManager, m_soundManager));
        }
    }
}

void WinState::update(float deltaTime)
{
    if (!m_winSoundPlayed) {
        m_winSoundPlayed = true;

        if (m_soundManager) {
            m_soundManager->playWorldClear();
        }
    }

    m_animTimer += deltaTime;

    // Gentle bounce animation for character sprites
    float bounce    = std::sin(m_animTimer * 4.0f) * 6.0f;
    float baselineY = 205.0f;
    float peachY    = baselineY + bounce;
    float heroY     = baselineY - bounce; // Counter-bounce for playful interaction

    if (m_config.mode == GameMode::TwoPlayer)
    {
        if (m_heroLoaded)   m_heroSprite.setPosition({ WIN_W / 2.0f - 100.0f, heroY });
        if (m_peachLoaded)  m_peachSprite.setPosition({ WIN_W / 2.0f - 15.0f, peachY });
        if (m_hero2Loaded)  m_hero2Sprite.setPosition({ WIN_W / 2.0f + 70.0f, heroY });
    }
    else
    {
        if (m_heroLoaded)   m_heroSprite.setPosition({ WIN_W / 2.0f - 55.0f, heroY });
        if (m_peachLoaded)  m_peachSprite.setPosition({ WIN_W / 2.0f + 25.0f, peachY });
    }

    // Static speech bubble position (does not bob up/down with Peach)
    float bubbleX = (m_config.mode == GameMode::TwoPlayer) ? (WIN_W / 2.0f + 160.0f) : (WIN_W / 2.0f + 180.0f);
    float bubbleY = 160.0f;

    m_bubbleBox.setPosition({ bubbleX, bubbleY });
    m_bubbleTail.setPosition({ bubbleX, bubbleY });
    m_bubbleText.setPosition({ bubbleX, bubbleY });

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

    // Peach speech bubble
    if (m_peachLoaded && m_fontLoaded)
    {
        window.draw(m_bubbleTail);
        window.draw(m_bubbleBox);
        window.draw(m_bubbleText);
    }

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
