#pragma once

#include "../interfaces/IHUDManager.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <string>
#include <functional>

namespace sf {
class RenderWindow;
class Event;
}

/**
 * @class HUDManager
 * @brief Full implementation of HUD/UI management
 *
 * Developer: Nguyen Phuc
 *
 * Displays: score, lives, item count, world/level, and an SMB-style
 * countdown timer. Includes small animations (score pop, timer blink
 * when low).
 *
 * Performance: strings are only rebuilt when their value actually
 * changes (dirty tracking), so a steady frame does zero string work.
 */
class HUDManager : public IHUDManager
{
public:
    HUDManager();
    ~HUDManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    void updateScore(int score) override;
    void updateLives(int lives) override;
    void updateItemCount(int count) override;
    void updateWorld(int level) override;
    void resetTimer() override;
    bool isTimeUp() const override;

    void setCharacter(CharacterType characterType) override;
    void showToast(const std::string& message, float duration = 2.0f) override;
    void spawnScorePopup(int points, float worldX, float worldY) override;
    void renderPopups(sf::RenderWindow& window, const sf::View* cameraView = nullptr) const override;
    void startTimerBonus(std::function<void(int bonus)> onScoreTick, std::function<void()> onComplete = nullptr, float worldX = 0.f, float worldY = 0.f) override;
    bool isTimerBonusActive() const override;
    float getTimeLeft() const override;

private:
    struct Toast
    {
        std::string message;
        float lifetime = 2.0f;
        float maxLifetime = 2.0f;
    };

    struct ScorePopup
    {
        sf::Vector2f position{0.0f, 0.0f};
        std::string textString;
        float lifetime = 0.75f;
        float maxLifetime = 0.75f;
        float velocityY = -60.0f; // px/s upward drift
        bool isOneUp = false;
    };

    void setupOutline(sf::Text& text) const;
    void updateScorePop(float deltaTime);
    void updateTimer(float deltaTime);
    void updateCoinAnim(float deltaTime);
    void updateToasts(float deltaTime);
    void updatePopups(float deltaTime);
    void updateTimerBonus(float deltaTime);
    void renderToasts(sf::RenderWindow& window) const;

    sf::Font m_font;

    // Label + value Texts are pairs so the layout is easy to tweak
    sf::Text m_scoreLabel{m_font};
    sf::Text m_coinsLabel{m_font};
    sf::Text m_livesLabel{m_font};
    sf::Text m_worldLabel{m_font};
    sf::Text m_timeLabel{m_font};
    sf::Text m_scoreValue{m_font};
    sf::Text m_coinsValue{m_font};
    sf::Text m_livesValue{m_font};
    sf::Text m_worldValue{m_font};
    sf::Text m_timeValue{m_font};

    // Semi-transparent bar behind the HUD
    sf::RectangleShape m_hudBar;

    // Animated coin icon
    std::array<sf::Texture, 4> m_coinTextures;
    sf::Sprite m_coinSprite{m_coinTextures[0]};
    int m_coinFrame = 0;
    float m_coinFrameTimer = 0.0f;
    static constexpr float COIN_FRAME_DUR = 0.25f; // 4 frames * 0.25s = 1.0s full rotation
    bool m_coinsLoaded = false;

    // Character life icon
    sf::Texture m_heroTexture;
    sf::Sprite m_heroSprite{m_heroTexture};
    bool m_heroLoaded = false;
    CharacterType m_characterType = CharacterType::Mario;

    // Last rendered values for dirty tracking
    int m_renderedScore = -1;
    int m_renderedLives = -1;
    int m_renderedItemCount = -1;
    int m_renderedTime = -1;
    int m_renderedWorld = -1;

    // SMB-style countdown timer (seconds), ticks down in update()
    float m_timeLeft = 300.0f;
    static constexpr float TIME_START = 300.0f;
    static constexpr float TIME_LOW = 60.0f;
    static constexpr float BLINK_INTERVAL = 0.4f;
    float m_blinkTimer = 0.0f;
    bool m_showTimerText = true;

    // Score "pop" animation state
    float m_scorePopTimer = 0.0f;
    bool m_scorePopActive = false;
    static constexpr float SCORE_POP_DURATION = 0.25f;
    static constexpr float PI = 3.14159265f;

    // Toasts
    std::vector<Toast> m_toasts;

    // Floating score popups
    std::vector<ScorePopup> m_popups;
    static constexpr float SCORE_POPUP_LIFETIME = 0.8f;
    static constexpr float SCORE_POPUP_DRIFT_SPEED = -50.0f;
    static constexpr int ONE_UP_THRESHOLD = 10000;

    // Timer bonus tally sequence
    bool m_bonusActive = false;
    bool m_bonusDraining = false;
    int m_bonusTallyTotal = 0;
    static constexpr float BONUS_DRAIN_RATE = 120.0f; // Seconds per real second
    static constexpr int BONUS_TICK_POINTS = 50;
    float m_bonusAccum = 0.0f;
    float m_bonusHoldTimer = 0.0f;
    static constexpr float BONUS_HOLD_DURATION = 0.9f;
    sf::Vector2f m_bonusWorldPos{ 0.f, 0.f };
    std::function<void(int)> m_bonusTickCallback;
    std::function<void()> m_bonusDoneCallback;

    bool m_fontLoaded = false;
};
