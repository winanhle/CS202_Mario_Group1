#pragma once
#include <SFML/Graphics.hpp>
#include "../../interfaces/IPlayerManager.h"
#include "../../interfaces/IMapManager.h"
#include "input/PlayerInputHandler.h"
#include "forms/IPlayerForm.h"
#include "forms/NormalForm.h"
#include "forms/SuperForm.h"
#include "forms/FireForm.h"
#include "../projectile/FireballManager.h"
#include "StarState.h"
#include <memory>
#include <string>

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
    
    // --- THÔNG SỐ RPG ---
    int m_maxHealth;
    int m_currentHealth;
    int m_attackPower;
    int m_defense;

    // ─── dependency ───
    IMapManager* m_mapManager = nullptr;
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
    int   m_score;
    bool  m_isAlive;
    bool  m_isGrounded;
    bool  m_isJumping;
    int   m_facingDirection = 1; // 1 = phải, -1 = trái — hướng nhìn, giữ nguyên khi đứng yên
    int   m_inputDirection  = 0; // -1/0/1 — hướng phím thô mỗi frame (0 = không bấm)
    int   m_playerIndex = 1;     // 1 = P1, 2 = P2
    bool  m_isTwoPlayerMode = false;

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
    float getPositionX() const override;
    float getPositionY() const override;
    void restoreState(int score, int lives, float posX, float posY) override;
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
    void setMapManager(IMapManager* map) override;

    /**
     * @brief Instantly kill the player (DEATH_ZONE contact), no form downgrade.
     */
    void die() override;

    /**
     * @brief Returns which power-up form the player is currently in.
     *        Used by MapManager to decide Mushroom vs FireFlower.
     */
    FormType getFormType() const override;
    void setFireballEnemyTarget(IEnemyManager* enemies) override;

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
};
