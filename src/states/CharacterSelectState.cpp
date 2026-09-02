#include "CharacterSelectState.h"
#include "ModeSelectState.h"
#include "MenuState.h"
#include "../core/StateManager.h"
#include "../interfaces/ISoundManager.h"
#include <SFML/Graphics.hpp>
#include <array>

// ─── Màu sắc ───────────────────────────────────────────────
static const sf::Color BG_TOP    = sf::Color(20,  20,  60);
static const sf::Color BG_BOT    = sf::Color(10,  10,  40);
static const sf::Color CARD_IDLE = sf::Color(40,  40, 100, 210);
static const sf::Color CARD_SEL  = sf::Color(80, 120, 220, 230);
static const sf::Color OUTLINE_SEL = sf::Color(160, 200, 255);
static const sf::Color OUTLINE_IDLE = sf::Color(60,  60, 140);

CharacterSelectState::CharacterSelectState(
    std::shared_ptr<ISettingsManager> settings,
    std::shared_ptr<ISaveManager> saveManager,
    std::shared_ptr<ISoundManager> soundManager,
    bool loadSave
)
    : m_fontLoaded(false)
    , m_preview{sf::Sprite(m_marioTexture), sf::Sprite(m_luigiTexture)}
    , m_settings(std::move(settings))
    , m_saveManager(std::move(saveManager))
    , m_soundManager(std::move(soundManager))
    , m_loadSave(loadSave)
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    // ── Tải texture preview ──────────────────────────────────
    if (m_marioTexture.loadFromFile("assets/texture/hero/mario.png"))
    {
        m_preview[0].setTexture(m_marioTexture);
        m_preview[0].setTextureRect(sf::IntRect({154, 30}, {16, 28}));
        m_preview[0].setScale({3.f, 3.f});
    }
    if (m_luigiTexture.loadFromFile("assets/texture/hero/luigi.png"))
    {
        m_preview[1].setTexture(m_luigiTexture);
        m_preview[1].setTextureRect(sf::IntRect({154, 29}, {16, 29}));
        m_preview[1].setScale({3.f, 3.f});
    }

    // ── Thẻ nhân vật ─────────────────────────────────────────
    const float CARD_W   = 220.f;
    const float CARD_H   = 280.f;
    const float CARD_Y   = 200.f;
    const float GAP      = 60.f;
    const float START_X  = WIN_W / 2.f - GAP / 2.f - CARD_W;

    const std::array<const char*, 2> names = {"MARIO", "LUIGI"};
    const std::array<const char*, 2> descs = {
        "Speed: 200\nJump : normal\nGravity: high",
        "Speed: 190\nJump : higher\nGravity: low"
    };

    for (int i = 0; i < 2; ++i)
    {
        float cx = START_X + i * (CARD_W + GAP);

        m_card[i].setSize({CARD_W, CARD_H});
        m_card[i].setPosition({cx, CARD_Y});
        m_card[i].setOutlineThickness(3.f);

        // Sprite preview (căn giữa thẻ theo chiều ngang, đặt chân ở cùng vị trí cố định)
        const auto rect = m_preview[i].getTextureRect();
        m_preview[i].setOrigin({ rect.size.x / 2.f, static_cast<float>(rect.size.y) });
        m_preview[i].setPosition({ cx + CARD_W / 2.f, CARD_Y + 120.f });

        if (m_fontLoaded)
        {
            m_charName[i].setFont(m_font);
            m_charName[i].setString(names[i]);
            m_charName[i].setCharacterSize(24);
            m_charName[i].setFillColor(sf::Color::White);
            sf::FloatRect nb = m_charName[i].getLocalBounds();
            m_charName[i].setOrigin({nb.position.x + nb.size.x / 2.f, nb.position.y});
            m_charName[i].setPosition({cx + CARD_W / 2.f, CARD_Y + 145.f});

            m_charDesc[i].setFont(m_font);
            m_charDesc[i].setString(descs[i]);
            m_charDesc[i].setCharacterSize(14);
            m_charDesc[i].setFillColor(sf::Color(200, 220, 255));
            m_charDesc[i].setPosition({cx + 15.f, CARD_Y + 185.f});
        }
    }

    // ── Tiêu đề ───────────────────────────────────────────────
    if (m_fontLoaded)
    {
        m_titleText.setFont(m_font);
        m_titleText.setString("CHOOSE YOUR CHARACTER");
        m_titleText.setCharacterSize(36);
        m_titleText.setFillColor(sf::Color::White);
        m_titleText.setOutlineColor(sf::Color::Black);
        m_titleText.setOutlineThickness(2.f);
        sf::FloatRect tb = m_titleText.getLocalBounds();
        m_titleText.setOrigin({tb.position.x + tb.size.x / 2.f,
                               tb.position.y + tb.size.y / 2.f});
        m_titleText.setPosition({WIN_W / 2.f, 100.f});

        m_hintText.setFont(m_font);
        m_hintText.setString("A/D or Left/Right to select   Enter to confirm   Esc to back");
        m_hintText.setCharacterSize(14);
        m_hintText.setFillColor(sf::Color(180, 180, 180));
        sf::FloatRect hb = m_hintText.getLocalBounds();
        m_hintText.setOrigin({hb.position.x + hb.size.x / 2.f, hb.position.y});
        m_hintText.setPosition({WIN_W / 2.f, 530.f});
    }

    m_nav.setAxis(UINavigator::Axis::Horizontal);
    m_nav.getHitbox = [this](int i) { return m_card[i].getGlobalBounds(); };
    m_nav.onActivate = [this](int) { confirm(); };
    m_nav.onSelectionChanged = [this](int, int) {
        if (m_soundManager) m_soundManager->playSelect();
        refreshUI();
    };

    refreshUI();
}

void CharacterSelectState::refreshUI()
{
    for (int i = 0; i < 2; ++i)
    {
        bool selected = (i == m_nav.getSelectedIndex());
        m_card[i].setFillColor(selected ? CARD_SEL : CARD_IDLE);
        m_card[i].setOutlineColor(selected ? OUTLINE_SEL : OUTLINE_IDLE);
        m_charName[i].setFillColor(selected ? sf::Color::Yellow : sf::Color::White);
    }
}

void CharacterSelectState::handleInput(const sf::Event& event)
{
    if (const auto* resizeEvent = event.getIf<sf::Event::Resized>())
    {
        m_windowSize = resizeEvent->size;
    }
    else if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape)
        {
            if (m_soundManager) m_soundManager->playSelect();
            if (auto* mgr = getStateManager())
                mgr->changeState(std::make_unique<MenuState>(m_settings, m_saveManager, m_soundManager));
            return;
        }
    }

    m_nav.handleInput(event, m_windowSize);
}

void CharacterSelectState::confirm()
{
    if (m_soundManager) m_soundManager->playStomp();

    m_config.player1Character = (m_nav.getSelectedIndex() == 0)
        ? CharacterType::Mario
        : CharacterType::Luigi;

    // Player 2 tự động là nhân vật còn lại
    m_config.player2Character = (m_nav.getSelectedIndex() == 0)
        ? CharacterType::Luigi
        : CharacterType::Mario;

    if (auto* mgr = getStateManager())
        mgr->changeState(std::make_unique<ModeSelectState>(m_config, m_settings, m_saveManager, m_loadSave, m_soundManager));
}

void CharacterSelectState::update(float deltaTime)
{
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= BLINK_INTERVAL)
    {
        m_blinkTimer = 0.f;
        m_blinkOn    = !m_blinkOn;
    }
}

void CharacterSelectState::render(sf::RenderWindow& window) const
{
    m_windowSize = window.getSize();

    // Nền gradient giả (2 hình chữ nhật)
    sf::RectangleShape bg({WIN_W, WIN_H});
    bg.setFillColor(BG_TOP);
    window.draw(bg);

    sf::RectangleShape bgBot({WIN_W, WIN_H / 2.f});
    bgBot.setPosition({0.f, WIN_H / 2.f});
    bgBot.setFillColor(BG_BOT);
    window.draw(bgBot);

    window.draw(m_titleText);

    for (int i = 0; i < 2; ++i)
    {
        window.draw(m_card[i]);
        window.draw(m_preview[i]);
        window.draw(m_charName[i]);
        window.draw(m_charDesc[i]);

        // Mũi tên chỉ vào thẻ được chọn (nhấp nháy)
        if (i == m_nav.getSelectedIndex() && m_blinkOn && m_fontLoaded)
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
