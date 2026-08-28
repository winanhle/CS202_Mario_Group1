#include "HUDManager.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iostream>

static constexpr sf::Color HUD_TEXT_COLOR = sf::Color::White;
static constexpr sf::Color HUD_TIME_WARNING_COLOR = sf::Color(255, 60, 60);
static constexpr sf::Color HUD_POPUP_SCORE_COLOR = sf::Color(255, 230, 40);
static constexpr sf::Color HUD_POPUP_1UP_COLOR = sf::Color(80, 240, 80);

HUDManager::HUDManager()
    : m_renderedScore(-1)
    , m_renderedLives(-1)
    , m_renderedItemCount(-1)
    , m_renderedTime(-1)
    , m_renderedWorld(-1)
    , m_timeLeft(TIME_START)
    , m_blinkTimer(0.0f)
    , m_showTimerText(true)
    , m_scorePopTimer(0.0f)
    , m_scorePopActive(false)
    , m_fontLoaded(false)
{
}

void HUDManager::initialize()
{
    m_fontLoaded = m_font.openFromFile("assets/fonts/SuperMario256.ttf");
    if (!m_fontLoaded)
    {
        std::cerr << "[HUDManager] ERROR: Failed to open font assets/fonts/SuperMario256.ttf\n";
    }

    auto setupOutline = [](sf::Text& text) {
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(1.5f);
    };

    // Load coin animation frames
    const std::array<std::string, 4> coinPaths = {
        "assets/texture/item/Coin.PNG",
        "assets/texture/item/Coin1.PNG",
        "assets/texture/item/Coin2.PNG",
        "assets/texture/item/Coin3.PNG"
    };
    m_coinsLoaded = true;
    for (std::size_t i = 0; i < 4; ++i)
    {
        if (!m_coinTextures[i].loadFromFile(coinPaths[i]))
        {
            m_coinsLoaded = false;
            std::cerr << "[HUDManager] ERROR: Failed to load coin texture: " << coinPaths[i] << "\n";
        }
    }
    if (m_coinsLoaded)
    {
        m_coinSprite.setTexture(m_coinTextures[0], true);
        sf::FloatRect cb = m_coinSprite.getLocalBounds();
        m_coinSprite.setOrigin({ cb.position.x + cb.size.x / 2.0f, cb.position.y + cb.size.y / 2.0f });
        m_coinSprite.setScale({ 0.82f, 0.82f });
        m_coinSprite.setPosition({ 224.0f, 43.0f });
    }

    // Load hero life icon (defaults to Mario)
    setCharacter(m_characterType);

    // 1. Player / Score (Col 1)
    m_scoreLabel.setFont(m_font);
    m_scoreLabel.setString(m_characterType == CharacterType::Luigi ? "LUIGI" : "MARIO");
    m_scoreLabel.setCharacterSize(20);
    m_scoreLabel.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_scoreLabel);
    m_scoreLabel.setPosition({ 25.0f, 8.0f });

    m_scoreValue.setFont(m_font);
    m_scoreValue.setString("000000");
    m_scoreValue.setCharacterSize(22);
    m_scoreValue.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_scoreValue);
    sf::FloatRect scoreBounds = m_scoreValue.getLocalBounds();
    m_scoreValue.setOrigin({ 0.0f, scoreBounds.position.y + scoreBounds.size.y / 2.0f });
    m_scoreValue.setPosition({ 25.0f, 40.0f });

    // 2. Coins (Col 2)
    m_coinsLabel.setFont(m_font);
    m_coinsLabel.setString("COINS");
    m_coinsLabel.setCharacterSize(20);
    m_coinsLabel.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_coinsLabel);
    m_coinsLabel.setPosition({ 210.0f, 8.0f });

    m_coinsValue.setFont(m_font);
    m_coinsValue.setString("x 00");
    m_coinsValue.setCharacterSize(22);
    m_coinsValue.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_coinsValue);
    m_coinsValue.setPosition({ 238.0f, 30.0f });

    // Lives Label
    m_livesLabel.setFont(m_font);
    m_livesLabel.setString("LIVES");
    m_livesLabel.setCharacterSize(20);
    m_livesLabel.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_livesLabel);
    m_livesLabel.setPosition({ 370.0f, 8.0f });

    m_livesValue.setFont(m_font);
    m_livesValue.setString("x 3");
    m_livesValue.setCharacterSize(22);
    m_livesValue.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_livesValue);
    m_livesValue.setPosition({ 405.0f, 30.0f });

    // World / level Label
    m_worldLabel.setFont(m_font);
    m_worldLabel.setString("WORLD");
    m_worldLabel.setCharacterSize(20);
    m_worldLabel.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_worldLabel);
    m_worldLabel.setPosition({ 540.0f, 8.0f });

    m_worldValue.setFont(m_font);
    m_worldValue.setString("1-1");
    m_worldValue.setCharacterSize(22);
    m_worldValue.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_worldValue);
    m_worldValue.setPosition({ 555.0f, 30.0f });

    // Timer Label (top-right)
    m_timeLabel.setFont(m_font);
    m_timeLabel.setString("TIME");
    m_timeLabel.setCharacterSize(20);
    m_timeLabel.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_timeLabel);
    m_timeLabel.setPosition({ 685.0f, 8.0f });

    m_timeValue.setFont(m_font);
    m_timeValue.setString("300");
    m_timeValue.setCharacterSize(22);
    m_timeValue.setFillColor(HUD_TEXT_COLOR);
    setupOutline(m_timeValue);
    m_timeValue.setPosition({ 690.0f, 30.0f });
}

void HUDManager::setCharacter(CharacterType characterType)
{
    m_characterType = characterType;
    m_scoreLabel.setString(characterType == CharacterType::Luigi ? "LUIGI" : "MARIO");
    const std::string heroTexPath = (characterType == CharacterType::Luigi)
        ? "assets/texture/hero/luigi.png"
        : "assets/texture/hero/mario.png";
    m_heroLoaded = m_heroTexture.loadFromFile(heroTexPath);
    if (m_heroLoaded)
    {
        m_heroSprite.setTexture(m_heroTexture);
        if (characterType == CharacterType::Luigi)
        {
            m_heroSprite.setTextureRect(sf::IntRect({ 1, 2 }, { 16, 23 }));
            m_heroSprite.setScale({ 1.3f, 1.3f });
            m_heroSprite.setPosition({ 375.0f, 26.0f });
        }
        else
        {
            m_heroSprite.setTextureRect(sf::IntRect({ 1, 6 }, { 16, 18 }));
            m_heroSprite.setScale({ 1.4f, 1.4f });
            m_heroSprite.setPosition({ 375.0f, 30.0f });
        }
    }
    else
    {
        std::cerr << "[HUDManager] ERROR: Failed to load hero texture: " << heroTexPath << "\n";
    }
}

void HUDManager::update(float deltaTime)
{
    updateTimer(deltaTime);
    updateScorePop(deltaTime);
    updateCoinAnim(deltaTime);
    updateToasts(deltaTime);
    updatePopups(deltaTime);
    updateTimerBonus(deltaTime);
}

void HUDManager::updateCoinAnim(float deltaTime)
{
    if (!m_coinsLoaded)
        return;
    m_coinFrameTimer += deltaTime;
    while (m_coinFrameTimer >= COIN_FRAME_DUR)
    {
        m_coinFrameTimer -= COIN_FRAME_DUR;
        m_coinFrame = (m_coinFrame + 1) % 4;
        m_coinSprite.setTexture(m_coinTextures[static_cast<std::size_t>(m_coinFrame)], true);
        sf::FloatRect cb = m_coinSprite.getLocalBounds();
        m_coinSprite.setOrigin({ cb.position.x + cb.size.x / 2.0f, cb.position.y + cb.size.y / 2.0f });
    }
}

void HUDManager::updateTimer(float deltaTime)
{
    if (!m_bonusActive && m_timeLeft > 0.0f)
    {
        m_timeLeft -= deltaTime;
        if (m_timeLeft < 0.0f)
            m_timeLeft = 0.0f;
    }
    int seconds = static_cast<int>(std::ceil(m_timeLeft));
    if (seconds != m_renderedTime)
    {
        m_renderedTime = seconds;
        std::string tStr = std::to_string(seconds);
        if (tStr.length() < 3)
            tStr = std::string(3 - tStr.length(), '0') + tStr;
        m_timeValue.setString(tStr);
    }
    // Low-time warning (< 60s): pulsing red/white
    if (m_timeLeft <= TIME_LOW && m_timeLeft > 0.0f)
    {
        m_blinkTimer += deltaTime;
        if (m_blinkTimer >= BLINK_INTERVAL)
        {
            m_blinkTimer = 0.0f;
            m_showTimerText = !m_showTimerText;
        }
        m_timeLabel.setFillColor(m_showTimerText ? HUD_TIME_WARNING_COLOR : HUD_TEXT_COLOR);
        m_timeValue.setFillColor(m_showTimerText ? HUD_TIME_WARNING_COLOR : HUD_TEXT_COLOR);
    }
    else
    {
        m_showTimerText = true;
        m_timeLabel.setFillColor(HUD_TEXT_COLOR);
        m_timeValue.setFillColor(HUD_TEXT_COLOR);
    }
}

void HUDManager::updateScorePop(float deltaTime)
{
    if (!m_scorePopActive)
        return;

    if (m_scorePopTimer > 0.0f)
    {
        m_scorePopTimer -= deltaTime;
        if (m_scorePopTimer < 0.0f)
            m_scorePopTimer = 0.0f;

        float progress = 1.0f - (m_scorePopTimer / SCORE_POP_DURATION);
        float scale = 1.0f + 0.3f * std::sin(progress * PI);
        m_scoreValue.setScale({ scale, scale });
    }
    else
    {
        // Pop finished: reset the scale exactly once
        m_scoreValue.setScale({ 1.0f, 1.0f });
        m_scorePopActive = false;
    }
}

void HUDManager::updateToasts(float deltaTime)
{
    for (auto it = m_toasts.begin(); it != m_toasts.end();)
    {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.0f)
        {
            it = m_toasts.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void HUDManager::updatePopups(float deltaTime)
{
    for (auto it = m_popups.begin(); it != m_popups.end();)
    {
        it->lifetime -= deltaTime;
        it->position.y += it->velocityY * deltaTime;
        if (it->lifetime <= 0.0f)
        {
            it = m_popups.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void HUDManager::updateTimerBonus(float deltaTime)
{
    if (!m_bonusActive)
        return;
    if (m_bonusDraining)
    {
        if (m_timeLeft > 0.0f)
        {
            float drain = BONUS_DRAIN_RATE * deltaTime;
            float actualDrain = std::min(m_timeLeft, drain);
            m_timeLeft -= actualDrain;
            m_bonusAccum += actualDrain;
            while (m_bonusAccum >= 1.0f)
            {
                m_bonusAccum -= 1.0f;
                m_bonusTallyTotal += BONUS_TICK_POINTS;
                if (m_bonusTickCallback)
                {
                    m_bonusTickCallback(BONUS_TICK_POINTS);
                }
            }
            if (m_timeLeft <= 0.0f)
            {
                m_timeLeft = 0.0f;
                m_bonusDraining = false;
                // Once rapid countdown finishes, spawn a floating score popup to drift up and fade!
                spawnScorePopup(m_bonusTallyTotal, m_bonusWorldPos.x, m_bonusWorldPos.y);
            }
        }
        else
        {
            m_bonusDraining = false;
            spawnScorePopup(m_bonusTallyTotal, m_bonusWorldPos.x, m_bonusWorldPos.y);
        }
    }
    else
    {
        m_bonusHoldTimer -= deltaTime;
        if (m_bonusHoldTimer <= 0.0f)
        {
            m_bonusActive = false;
            if (m_bonusDoneCallback)
            {
                m_bonusDoneCallback();
            }
        }
    }
}

void HUDManager::render(sf::RenderWindow& window) const
{
    if (!m_fontLoaded)
        return;

    // Draw semi-transparent HUD background bar


    // Draw all HUD text
    window.draw(m_scoreLabel);
    window.draw(m_scoreValue);
    window.draw(m_coinsLabel);
    if (m_coinsLoaded)
        window.draw(m_coinSprite);
    window.draw(m_coinsValue);
    window.draw(m_livesLabel);
    if (m_heroLoaded)
        window.draw(m_heroSprite);
    window.draw(m_livesValue);
    window.draw(m_worldLabel);
    window.draw(m_worldValue);
    window.draw(m_timeLabel);
    window.draw(m_timeValue);
    // Draw active toast notifications (in screen space)
    renderToasts(window);
}

void HUDManager::renderToasts(sf::RenderWindow& window) const
{
    if (m_toasts.empty() || !m_fontLoaded)
        return;
    float currentY = 75.0f;
    for (const auto& toast : m_toasts)
    {
        float alphaFactor = 1.0f;
        if (toast.lifetime < 0.4f)
        {
            alphaFactor = std::max(0.0f, toast.lifetime / 0.4f);
        }
        else if (toast.maxLifetime - toast.lifetime < 0.2f)
        {
            alphaFactor = std::max(0.0f, (toast.maxLifetime - toast.lifetime) / 0.2f);
        }
        auto alphaUint = static_cast<std::uint8_t>(std::clamp(alphaFactor * 255.0f, 0.0f, 255.0f));
        sf::Text toastText{m_font};
        toastText.setString(toast.message);
        toastText.setCharacterSize(18);
        toastText.setFillColor(sf::Color(255, 255, 255, alphaUint));
        
        sf::FloatRect bounds = toastText.getLocalBounds();
        float boxW = bounds.size.x + 24.0f;
        float boxH = bounds.size.y + 16.0f;
        float posX = 800.0f - boxW - 20.0f;
        sf::RectangleShape box({ boxW, boxH });
        box.setFillColor(sf::Color(20, 20, 20, static_cast<std::uint8_t>(alphaFactor * 200.0f)));
        box.setOutlineColor(sf::Color(240, 200, 60, alphaUint));
        box.setOutlineThickness(1.5f);
        box.setPosition({ posX, currentY });
        toastText.setPosition({ posX + 12.0f, currentY + 6.0f });
        window.draw(box);
        window.draw(toastText);
        currentY += boxH + 8.0f;
    }
}

void HUDManager::renderPopups(sf::RenderWindow& window, const sf::View* cameraView) const
{
    if (!m_fontLoaded)
        return;
    // 1. Draw regular floating popups
    for (const auto& popup : m_popups)
    {
        float alphaFactor = std::clamp(popup.lifetime / popup.maxLifetime, 0.0f, 1.0f);
        auto alphaUint = static_cast<std::uint8_t>(alphaFactor * 255.0f);
        sf::Vector2f drawPos = popup.position;
        if (cameraView)
        {
            // Map world-space position through the camera view to get screen coordinates
            sf::Vector2i pixelPos = window.mapCoordsToPixel(popup.position, *cameraView);
            drawPos = window.mapPixelToCoords(pixelPos, window.getDefaultView());
        }
        sf::Text text{m_font};
        text.setString(popup.textString);
        text.setCharacterSize(20);
        text.setFillColor(popup.isOneUp
            ? sf::Color(HUD_POPUP_1UP_COLOR.r, HUD_POPUP_1UP_COLOR.g, HUD_POPUP_1UP_COLOR.b, alphaUint)
            : sf::Color(HUD_POPUP_SCORE_COLOR.r, HUD_POPUP_SCORE_COLOR.g, HUD_POPUP_SCORE_COLOR.b, alphaUint));
        text.setOutlineColor(sf::Color(0, 0, 0, alphaUint));
        text.setOutlineThickness(1.5f);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                         bounds.position.y + bounds.size.y / 2.0f });
        text.setPosition(drawPos);
        window.draw(text);
    }
    // 2. Draw actively accumulating timer score popup in world space above the player
    if (m_bonusActive && m_bonusDraining && m_bonusTallyTotal > 0)
    {
        sf::Vector2f drawPos = m_bonusWorldPos;
        if (cameraView)
        {
            sf::Vector2i pixelPos = window.mapCoordsToPixel(m_bonusWorldPos, *cameraView);
            drawPos = window.mapPixelToCoords(pixelPos, window.getDefaultView());
        }
        sf::Text text{m_font};
        text.setString("+" + std::to_string(m_bonusTallyTotal));
        text.setCharacterSize(22);
        text.setFillColor(HUD_POPUP_SCORE_COLOR);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(1.5f);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({ bounds.position.x + bounds.size.x / 2.0f,
                         bounds.position.y + bounds.size.y / 2.0f });
        text.setPosition(drawPos);
        window.draw(text);
    }
}

void HUDManager::handleInput(const sf::Event& event)
{
    // HUD input handling (for future menu interactions)
    (void)event;
}

void HUDManager::updateScore(int score)
{
    if (score != m_renderedScore)
    {
        m_renderedScore = score;
        std::string s = std::to_string(std::max(0, score));
        if (s.length() < 6)
            s = std::string(6 - s.length(), '0') + s;
        m_scoreValue.setString(s);
        m_scorePopTimer = SCORE_POP_DURATION;
        m_scorePopActive = true;
    }
}

void HUDManager::updateLives(int lives)
{
    if (lives != m_renderedLives)
    {
        m_renderedLives = lives;
        m_livesValue.setString("x " + std::to_string(std::max(0, lives)));
    }
}

void HUDManager::updateItemCount(int count)
{
    if (count != m_renderedItemCount)
    {
        m_renderedItemCount = count;
        std::string c = std::to_string(std::max(0, count));
        if (c.length() < 2)
            c = "0" + c;
        m_coinsValue.setString("x " + c);
    }
}

bool HUDManager::isTimeUp() const
{
    return m_timeLeft <= 0.0f;
}

void HUDManager::updateWorld(int level)
{
    if (level != m_renderedWorld)
    {
        m_renderedWorld = level;
        m_worldValue.setString("1-" + std::to_string(level));
    }
}

void HUDManager::resetTimer()
{
    m_timeLeft = TIME_START;
    m_renderedTime = -1; // Force rebuild next update
    m_blinkTimer = 0.0f;
    m_showTimerText = true;
    m_bonusActive = false;
    m_bonusDraining = false;
    m_bonusTallyTotal = 0;
    m_toasts.clear();
    m_timeLabel.setFillColor(HUD_TEXT_COLOR);
    m_timeValue.setFillColor(HUD_TEXT_COLOR);
}

void HUDManager::showToast(const std::string& message, float duration)
{
    for (auto& existing : m_toasts)
    {
        if (existing.message == message)
        {
            existing.lifetime = duration;
            existing.maxLifetime = duration;
            return;
        }
    }
    Toast t;
    t.message = message;
    t.lifetime = duration;
    t.maxLifetime = duration;
    m_toasts.push_back(t);
}

void HUDManager::spawnScorePopup(int points, float worldX, float worldY)
{
    ScorePopup p;
    p.position = { worldX, worldY };
    p.lifetime = SCORE_POPUP_LIFETIME;
    p.maxLifetime = SCORE_POPUP_LIFETIME;
    p.velocityY = SCORE_POPUP_DRIFT_SPEED;
    if (points >= ONE_UP_THRESHOLD)
    {
        p.textString = "1-UP";
        p.isOneUp = true;
    }
    else
    {
        p.textString = "+" + std::to_string(points);
        p.isOneUp = false;
    }
    m_popups.push_back(p);
}

void HUDManager::startTimerBonus(std::function<void(int bonus)> onScoreTick, std::function<void()> onComplete, float worldX, float worldY)
{
    m_bonusActive = true;
    m_bonusDraining = true;
    m_bonusTallyTotal = 0;
    m_bonusAccum = 0.0f;
    m_bonusWorldPos = { worldX, worldY };
    m_bonusHoldTimer = BONUS_HOLD_DURATION;
    m_bonusTickCallback = std::move(onScoreTick);
    m_bonusDoneCallback = std::move(onComplete);
}

bool HUDManager::isTimerBonusActive() const
{
    return m_bonusActive;
}

float HUDManager::getTimeLeft() const
{
    return m_timeLeft;
}

