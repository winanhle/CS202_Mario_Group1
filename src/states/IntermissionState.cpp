#include "IntermissionState.h"
#include "../core/StateManager.h"
#include <iostream>

static void centerOrigin(sf::Text& text)
{
    const auto bounds = text.getLocalBounds();
    text.setOrigin({ bounds.position.x + bounds.size.x / 2.f,
                     bounds.position.y + bounds.size.y / 2.f });
}

IntermissionState::IntermissionState(const GameConfig& config,
                                     int currentStage,
                                     int nextStage,
                                     int sharedLives,
                                     std::function<void()> onProceed)
    : m_config(config)
    , m_currentStage(currentStage)
    , m_nextStage(nextStage)
    , m_sharedLives(sharedLives)
    , m_onProceed(std::move(onProceed))
{
    m_background.setFillColor(sf::Color::Black);
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");

    std::string p1TexPath = (m_config.player1Character == CharacterType::Luigi)
        ? "assets/texture/hero/luigi.png"
        : "assets/texture/hero/mario.png";

    m_heroLoaded = m_heroTexture.loadFromFile(p1TexPath);
    if (m_heroLoaded)
    {
        m_heroSprite.setTexture(m_heroTexture);
        if (m_config.player1Character == CharacterType::Luigi)
        {
            m_heroSprite.setTextureRect(sf::IntRect({1, 2}, {16, 23}));
            m_heroSprite.setOrigin({8.0f, 23.0f});
        }
        else
        {
            m_heroSprite.setTextureRect(sf::IntRect({1, 6}, {16, 18}));
            m_heroSprite.setOrigin({8.0f, 18.0f});
        }
        m_heroSprite.setScale({3.0f, 3.0f});
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
                m_hero2Sprite.setOrigin({8.0f, 18.0f});
            }
            else
            {
                m_hero2Sprite.setTextureRect(sf::IntRect({1, 2}, {16, 23}));
                m_hero2Sprite.setOrigin({8.0f, 23.0f});
            }
            m_hero2Sprite.setScale({3.0f, 3.0f});
        }

        if (m_heroLoaded)
            m_heroSprite.setPosition({ SCREEN_W / 2.0f - 75.0f, 345.0f });
        if (m_hero2Loaded)
            m_hero2Sprite.setPosition({ SCREEN_W / 2.0f - 25.0f, 345.0f });
    }
    else
    {
        if (m_heroLoaded)
            m_heroSprite.setPosition({ SCREEN_W / 2.0f - 40.0f, 345.0f });
    }

    if (m_fontLoaded)
    {
        // Stage Clear Text
        m_clearText.setFont(m_font);
        m_clearText.setString("STAGE " + std::to_string(m_currentStage) + " CLEAR!");
        m_clearText.setCharacterSize(34);
        m_clearText.setFillColor(sf::Color(255, 220, 50));
        m_clearText.setOutlineColor(sf::Color::Black);
        m_clearText.setOutlineThickness(3.0f);
        centerOrigin(m_clearText);
        m_clearText.setPosition({ SCREEN_W / 2.0f, 150.0f });

        // World Text
        m_worldText.setFont(m_font);
        m_worldText.setString("WORLD 1-" + std::to_string(m_nextStage));
        m_worldText.setCharacterSize(26);
        m_worldText.setFillColor(sf::Color::White);
        centerOrigin(m_worldText);
        m_worldText.setPosition({ SCREEN_W / 2.0f, 240.0f });

        // Lives Text
        m_livesText.setFont(m_font);
        m_livesText.setString("  x  " + std::to_string(m_sharedLives));
        m_livesText.setCharacterSize(24);
        m_livesText.setFillColor(sf::Color::White);
        const auto lb = m_livesText.getLocalBounds();
        m_livesText.setOrigin({ lb.position.x,
                                lb.position.y + lb.size.y / 2.0f });
        
        float livesX = (m_config.mode == GameMode::TwoPlayer)
            ? (SCREEN_W / 2.0f + 15.0f)
            : (SCREEN_W / 2.0f);
        m_livesText.setPosition({ livesX, 330.0f });

        // Hint Text
        m_hintText.setFont(m_font);
        m_hintText.setString("Press ENTER, SPACE or Click to continue");
        m_hintText.setCharacterSize(14);
        m_hintText.setFillColor(sf::Color(160, 170, 200));
        centerOrigin(m_hintText);
        m_hintText.setPosition({ SCREEN_W / 2.0f, 520.0f });
    }
}

void IntermissionState::handleInput(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Enter ||
            key->code == sf::Keyboard::Key::Space)
        {
            proceed();
        }
    }
    else if (const auto* mouse = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouse->button == sf::Mouse::Button::Left)
        {
            proceed();
        }
    }
}

void IntermissionState::update(float deltaTime)
{
    m_timer += deltaTime;
    if (m_timer >= DURATION)
    {
        proceed();
    }
}

void IntermissionState::proceed()
{
    if (m_proceeded)
        return;
    m_proceeded = true;

    if (m_onProceed)
    {
        m_onProceed();
    }

    auto* manager = getStateManager();
    if (manager)
    {
        manager->popState();
    }
}

void IntermissionState::render(sf::RenderWindow& window) const
{
    sf::View view({ SCREEN_W / 2.0f, SCREEN_H / 2.0f }, { SCREEN_W, SCREEN_H });
    window.setView(view);

    window.draw(m_background);

    if (m_fontLoaded)
    {
        window.draw(m_clearText);
        window.draw(m_worldText);
        window.draw(m_livesText);
        window.draw(m_hintText);
    }

    if (m_heroLoaded)
    {
        window.draw(m_heroSprite);
    }

    if (m_config.mode == GameMode::TwoPlayer && m_hero2Loaded)
    {
        window.draw(m_hero2Sprite);
    }
}
