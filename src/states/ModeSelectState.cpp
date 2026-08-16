#include "ModeSelectState.h"
#include "PlayState.h"
#include "CharacterSelectState.h"
#include "../core/StateManager.h"
#include <SFML/Graphics.hpp>

// ─── Màu sắc ───────────────────────────────────────────────
static const sf::Color MS_BG_TOP    = sf::Color(20,  50,  20);
static const sf::Color MS_BG_BOT    = sf::Color(10,  30,  10);
static const sf::Color MS_CARD_IDLE = sf::Color(30,  80,  30, 210);
static const sf::Color MS_CARD_SEL  = sf::Color(60, 160,  60, 230);
static const sf::Color MS_OUT_SEL   = sf::Color(130, 230, 130);
static const sf::Color MS_OUT_IDLE  = sf::Color(40,  90,  40);

ModeSelectState::ModeSelectState(const GameConfig& config, std::shared_ptr<ISettingsManager> settings, bool loadSave)
    : m_config(config)
    , m_selectedIndex(0)
    , m_fontLoaded(false)
    , m_settings(std::move(settings))
    , m_loadSave(loadSave)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    // ── Tiêu đề ────────────────────────────────────────────────
    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("SELECT GAME MODE");
        m_titleText.setCharacterSize(36);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.f);
        sf::FloatRect tb = m_titleText.getLocalBounds();
        m_titleText.setOrigin({tb.position.x + tb.size.x / 2.f,
                               tb.position.y + tb.size.y / 2.f});
        m_titleText.setPosition({WIN_W / 2.f, 90.f});

        // Hiển thị nhân vật đã chọn
        const char* charName = (m_config.player1Character == CharacterType::Mario)
            ? "MARIO" : "LUIGI";
        m_subtitleText.setFont(m_font);
        m_subtitleText.setString(std::string("Player 1: ") + charName);
        m_subtitleText.setCharacterSize(20);
        m_subtitleText.setFillColor(sf::Color(200, 255, 200));
        sf::FloatRect sb = m_subtitleText.getLocalBounds();
        m_subtitleText.setOrigin({sb.position.x + sb.size.x / 2.f, sb.position.y});
        m_subtitleText.setPosition({WIN_W / 2.f, 145.f});
    }

    // ── Thẻ mode ───────────────────────────────────────────────
    const float CARD_W  = 230.f;
    const float CARD_H  = 220.f;
    const float CARD_Y  = 210.f;
    const float GAP     = 60.f;
    const float START_X = WIN_W / 2.f - GAP / 2.f - CARD_W;

    const char* modeNames[2] = {"1 PLAYER", "2 PLAYERS"};
    const char* modeDescs[2] = {
        "Solo adventure\n\nAll keys available\nW/Up/Space: jump\nA/Left: left\nD/Right: right",
        "Local co-op\n\nPlayer 1: WASD+Space\nPlayer 2: Arrow keys\n\nShared lives pool"
    };

    for (int i = 0; i < 2; ++i)
    {
        float cx = START_X + i * (CARD_W + GAP);

        m_card[i].setSize({CARD_W, CARD_H});
        m_card[i].setPosition({cx, CARD_Y});
        m_card[i].setOutlineThickness(3.f);

        if (m_fontLoaded)
        {
            m_modeName[i].setFont(m_font);
            m_modeName[i].setString(modeNames[i]);
            m_modeName[i].setCharacterSize(22);
            m_modeName[i].setFillColor(sf::Color::White);
            sf::FloatRect nb = m_modeName[i].getLocalBounds();
            m_modeName[i].setOrigin({nb.position.x + nb.size.x / 2.f, nb.position.y});
            m_modeName[i].setPosition({cx + CARD_W / 2.f, CARD_Y + 18.f});

            m_modeDesc[i].setFont(m_font);
            m_modeDesc[i].setString(modeDescs[i]);
            m_modeDesc[i].setCharacterSize(13);
            m_modeDesc[i].setFillColor(sf::Color(200, 230, 200));
            m_modeDesc[i].setPosition({cx + 14.f, CARD_Y + 60.f});
        }
    }

    // ── Hint ───────────────────────────────────────────────────
    if (m_fontLoaded)
    {
        m_hintText.setFont(m_font);
        m_hintText.setString("A/D or Left/Right to select   Enter to start   Esc to back");
        m_hintText.setCharacterSize(14);
        m_hintText.setFillColor(sf::Color(180, 180, 180));
        sf::FloatRect hb = m_hintText.getLocalBounds();
        m_hintText.setOrigin({hb.position.x + hb.size.x / 2.f, hb.position.y});
        m_hintText.setPosition({WIN_W / 2.f, 530.f});
    }

    refreshUI();
}

void ModeSelectState::refreshUI()
{
    for (int i = 0; i < 2; ++i)
    {
        bool sel = (i == m_selectedIndex);
        m_card[i].setFillColor(sel ? MS_CARD_SEL : MS_CARD_IDLE);
        m_card[i].setOutlineColor(sel ? MS_OUT_SEL : MS_OUT_IDLE);
        m_modeName[i].setFillColor(sel ? sf::Color::Yellow : sf::Color::White);
    }
}

void ModeSelectState::handleInput(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::Left:
        case sf::Keyboard::Key::A:
            m_selectedIndex = 0;
            refreshUI();
            break;

        case sf::Keyboard::Key::Right:
        case sf::Keyboard::Key::D:
            m_selectedIndex = 1;
            refreshUI();
            break;

        case sf::Keyboard::Key::Enter:
            confirm();
            break;

        case sf::Keyboard::Key::Escape:
            if (auto* mgr = getStateManager())
                mgr->changeState(std::make_unique<CharacterSelectState>(m_settings, m_loadSave));
            break;

        default:
            break;
        }
    }
}

void ModeSelectState::confirm()
{
    m_config.mode = (m_selectedIndex == 0)
        ? GameMode::SinglePlayer
        : GameMode::TwoPlayer;

    if (auto* mgr = getStateManager())
        mgr->changeState(std::make_unique<PlayState>(m_config, m_settings, m_loadSave));
}

void ModeSelectState::update(float deltaTime)
{
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL)
    {
        m_blinkTimer = 0.f;
        m_blinkOn    = !m_blinkOn;
    }
}

void ModeSelectState::render(sf::RenderWindow& window) const
{
    sf::RectangleShape bg({WIN_W, WIN_H});
    bg.setFillColor(MS_BG_TOP);
    window.draw(bg);

    sf::RectangleShape bgBot({WIN_W, WIN_H / 2.f});
    bgBot.setPosition({0.f, WIN_H / 2.f});
    bgBot.setFillColor(MS_BG_BOT);
    window.draw(bgBot);

    window.draw(m_titleText);
    window.draw(m_subtitleText);

    for (int i = 0; i < 2; ++i)
    {
        window.draw(m_card[i]);
        window.draw(m_modeName[i]);
        window.draw(m_modeDesc[i]);

        if (i == m_selectedIndex && m_blinkOn && m_fontLoaded)
        {
            sf::Text arrow(m_font);
            arrow.setString("▼");
            arrow.setCharacterSize(20);
            arrow.setFillColor(sf::Color::Yellow);
            sf::FloatRect ab = arrow.getLocalBounds();
            arrow.setOrigin({ab.position.x + ab.size.x / 2.f, ab.position.y});
            arrow.setPosition({m_card[i].getPosition().x + m_card[i].getSize().x / 2.f,
                               m_card[i].getPosition().y + m_card[i].getSize().y + 8.f});
            window.draw(arrow);
        }
    }

    window.draw(m_hintText);
}
