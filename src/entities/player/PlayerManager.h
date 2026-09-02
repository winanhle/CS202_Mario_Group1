#pragma once
#include <SFML/Graphics.hpp>
#include "../../interfaces/IPlayerManager.h"
#include "../../interfaces/IMapManager.h"
#include "../../interfaces/ILiftManager.h"
#include "input/PlayerInputHandler.h"
#include "forms/IPlayerForm.h"
#include "forms/NormalForm.h"
#include "forms/SuperForm.h"
#include "forms/FireForm.h"
#include "../projectile/FireballManager.h"
#include "StarState.h"
#include <memory>
#include <string>
#include <functional>

class ISoundManager;

class PlayerManager : public IPlayerManager
{
protected:
    // --- THÔNG SỐ VẬT LÝ & DI CHUYỂN ---
    float m_maxSpeed;          // WALK max speed
    float m_runMaxSpeed;       // RUN max speed (khi giữ phím chạy)
    float m_acceleration;      // gia tốc cùng hướng (đất, cả walk & run)
    float m_deceleration;      // ma sát tuyến tính khi thả phím (chậm hơn accel)
    float m_skidDeceleration;  // phanh gấp khi bấm ngược hướng (2-3 lần accel)
    float m_airAcceleration;   // air control rất yếu — chỉ "nắn" quỹ đạo
    float m_jumpVelocity;
    float m_gravity;

    bool m_isSkidding = false; // true khi đang phanh gấp (skid) — dùng cho animation

    // ── Variable jump (nhấn GIỮ nút nhảy → nhảy cao hơn, tới giới hạn) ──
    float m_jumpHoldGrace = 0.30f; // giới hạn thời gian giữ (s) để tăng thêm độ cao
    float m_jumpHoldBoost = 0.30f; // tỉ lệ GIẢM trọng lực trong đà lên khi đang giữ
    float m_jumpHoldTimer = 0.f;   // thời gian đã giữ trong lần nhảy hiện tại
    bool  m_jumpSoundPlayedBig = false; // true khi đã phát âm thanh giữ nhảy (jump.wav)
    
    // --- THÔNG SỐ RPG ---
    int m_maxHealth;
    int m_currentHealth;
    int m_attackPower;
    int m_defense;

    // ─── dependency ───
    // --- TRẠNG THÁI HIỆN TẠI ---
    IMapManager*  m_mapManager  = nullptr;
    ILiftManager* m_liftManager = nullptr;
    ISoundManager* m_soundManager = nullptr;
    bool m_isInvincible = false;
    float m_invincibilityTimer = 0.f;
    bool m_isInitialized       = false;
    static constexpr float INVINCIBILITY_DURATION = 2.0f; // seconds of i-frames after a hit

    // ─── Star ───
    std::unique_ptr<StarState> m_starState; // null = bình thường

    // ─── tile collision ───
    void tileCollisionX(float deltaTime);
    void tileCollisionY(float deltaTime);

    /**
     * @brief Vật lý di chuyển ngang kiểu Mario NES: tăng tốc dần trên đất,
     * giữ động lượng trên không (không đảo hướng được khi đang nhảy);
     * friction khi đảo chiều nhanh hơn khi thả phím tự trôi.
     */
    void applyHorizontalPhysics(float deltaTime);

    // --- TRẠNG THÁI HIỆN TẠI ---
    float m_positionX, m_positionY;
    float m_spawnX, m_spawnY;   // vị trí spawn ban đầu (dùng cho respawn)
    float m_velocityX, m_velocityY;
    int   m_score = 0;
    int   m_coins = 0;
    int   m_pendingOneUps = 0;
    bool  m_isAlive = true;
    bool  m_isGrounded;
    bool  m_isJumping;
    int   m_facingDirection = 1; // 1 = phải, -1 = trái — hướng nhìn, giữ nguyên khi đứng yên
    int   m_inputDirection  = 0; // -1/0/1 — hướng phím thô mỗi frame (0 = không bấm)
    int   m_playerIndex = 1;     // 1 = P1, 2 = P2
    bool  m_isTwoPlayerMode = false;

    // ─── Death animation state machine ────────────────────────────────────────
    enum class DeathState { None, Animating, Done };
    DeathState m_deathState  = DeathState::None;
    float      m_deathVelY   = 0.f;
    float      m_deathTimer  = 0.f;
    float      m_deathStartY = 0.f; // world Y at death moment — used to detect off-screen

    static constexpr float DEATH_FREEZE_TIME =  0.35f; // brief pause with death pose before bounce
    static constexpr float DEATH_POPUP_VY    = -300.f; // initial upward bounce (px/s)
    static constexpr float DEATH_GRAVITY     =  950.f; // gravity during arc (px/s²)
    static constexpr float DEATH_ANIM_LIMIT  =   3.5f; // safety timeout (s)

    // --- PATTERNS ---
    std::unique_ptr<PlayerInputHandler> m_inputHandler;
    std::unique_ptr<IPlayerForm>        m_currentForm;

    // ─── FIREBALL ───
    std::unique_ptr<FireballManager> m_fireballManager = std::make_unique<FireballManager>();

    // --- SPRITE & ANIMATION ---
    sf::Texture  m_playerTexture;
    sf::Sprite   m_playerSprite;
    sf::Vector2f m_playerSize;
    int   m_currentFrame = 0;
    float m_animTimer    = 0.f;

    void updateAnimation(float deltaTime);
    void resetToStart();   // respawn tại vị trí spawn

    /**
     * @brief Trả về đường dẫn texture phù hợp với nhân vật.
     * Subclass (Mario, Luigi, ...) override để cung cấp path đúng.
     */
    virtual std::string getTexturePath() const = 0;

    virtual void setupStats() = 0;

private:
    // Builds a KeyBinding from settings (or hardcoded defaults) and
    // recreates the input handler so rebinds take effect immediately.
    void rebuildKeyBindings(ISettingsManager* settings);

public:
    PlayerManager();
    ~PlayerManager() override = default;

    void initialize(ISettingsManager* settings = nullptr) override;
    void setPlayerIndex(int index) override;
    void setTwoPlayerMode(bool isTwoPlayer) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    bool  isAlive()     const override;
    int   getScore()    const override;
    void  addScore(int points) override;
    int   getCoins()    const override { return m_coins; }
    void  setCoins(int coins) override { m_coins = coins; }
    float getPositionX() const override;
    float getPositionY() const override;
    bool  isDownPressed() const override;
    void  startPipeTravel(float alignX, float alignY, float targetX, float targetY) override;
    bool  isPipeTraveling() const override { return m_isPipeTraveling; }
    void restoreState(int score, int coins) override;
    void setSpawnPoint(float x, float y) override;

    void jump();
    void stopJump();
    void stopHorizontal();
    void moveLeft();
    void moveRight();

    /**
     * @brief Bắn cầu lửa (chỉ có tác dụng khi đang FireForm VÀ cooldown xong).
     * Được gọi bởi ShootCommand.
     */
    void shoot();

    void setForm(std::unique_ptr<IPlayerForm> newForm);
    void collectPowerUp(int type) override;

    sf::FloatRect getHitbox()   const override;
    float getVelocityY()        const override { return m_velocityY; }
    bool  isInvincible()        const override { return m_isInvincible; }
    void takeDamage()           override;
    void bounce()               override;
    void collectCoin(int amount) override;
    int  consumePendingOneUps() override;
    void setMapManager(IMapManager* map) override;
    void setSoundManager(ISoundManager* sound) override;
    void setLiftManager(ILiftManager* lifts) override { m_liftManager = lifts; }

    /**
     * @brief Instantly kill the player (DEATH_ZONE contact), no form downgrade.
     */
    void die() override;

    /**
     * @brief Launches the death arc animation and plays death.wav / gameover.wav.
     * @param isLastLife pass true when m_sharedLives <= 1 so gameover.wav plays.
     */
    void startDeathAnimation(bool isLastLife) override;

    /** True while the death pop arc is in flight. */
    bool isDeathAnimating() const override;

    /** True once the arc has finished (fallen offscreen or timed out). */
    bool isDeathDone() const override;

    /**
     * @brief Returns which power-up form the player is currently in.
     *        Used by MapManager to decide Mushroom vs FireFlower.
     */
    FormType getFormType() const override;
    void resetForm() override = 0;
    void setFireballEnemyTarget(IEnemyManager* enemies) override;

    // ─── Observer callbacks (career stat tracking) ──────────────────────────
    /** Called when coins are collected. Args: coin amount. */
    void setCoinCallback(std::function<void(int)> cb) { m_coinCallback = std::move(cb); }
    /** Called when a powerup changes the player's form. Args: FormType. */
    void setPowerUpCallback(std::function<void(FormType)> cb) { m_powerUpCallback = std::move(cb); }
    /** Called when a Star is collected (via activateStar). */
    void setStarCallback(std::function<void()> cb) { m_starCallback = std::move(cb); }

    /**
     * @brief Gán KeyBinding cho player (gọi sau initialize, trước game loop).
     * Dùng KeyBindingPresets để lấy preset phù hợp với chế độ chơi.
     */
    void setKeyBinding(const KeyBinding& keys);

    /**
     * @brief Hồi sinh player tại vị trí spawn, giữ nguyên form hiện tại.
     * Được gọi bởi GameWorld khi shared lives pool còn > 0.
     */
    void respawn() override;

    /** Kích hoạt StarState 10 giây. */
    void activateStar() override;

    /** Trả về true nếu StarState đang còn hiệu lực. */
    bool isStarActive() const override;

    void startFlagpoleSlide(float poleX) override;
    void startWalkToTarget(float targetX, bool disappearAtEnd = true) override;
    bool isFlagpoleSliding() const override { return m_isFlagpoleSliding; }
    bool isWalkingToCastle() const override { return m_isWalkingToCastle; }
    bool isInsideCastle() const override { return m_isInsideCastle; }
    bool hasFinishedFlagpole() const override { return m_hasFinishedFlagpole; }

    /**
     * @brief Offset position by (dx, dy) — called by LiftManager when player rides a lift.
     * Zeroes vertical velocity and marks the player grounded while riding.
     */
    void applyLiftOffset(float dx, float dy) override;

private:
    // ─── Observer callbacks ─────────────────────────────────────────────────
    std::function<void(int)>      m_coinCallback;
    std::function<void(FormType)> m_powerUpCallback;
    std::function<void()>         m_starCallback;

    bool  m_isPipeTraveling = false;
    bool  m_isPipeAligning = false;
    float m_pipeAlignX = 0.f;
    float m_pipeAlignY = 0.f;
    float m_pipeTargetX = 0.f;
    float m_pipeTargetY = 0.f;
    static constexpr float PIPE_ALIGN_SPEED = 80.f;
    static constexpr float PIPE_TRAVEL_SPEED = 36.f;

    bool  m_isFlagpoleSliding = false;
    bool  m_isWalkingToCastle = false;
    bool  m_isInsideCastle = false;
    bool  m_hasFinishedFlagpole = false;
    bool  m_disappearAtCastleEnd = true;
    float m_flagpoleFinishTimer = 0.f;
    float m_castleWalkTargetX = 0.f;
};
