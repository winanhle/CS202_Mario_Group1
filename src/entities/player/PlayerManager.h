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
#include <memory>
#include <string>

class PlayerManager : public IPlayerManager
{
protected:
    // --- THÔNG SỐ VẬT LÝ & DI CHUYỂN ---
    float m_maxSpeed;
    float m_acceleration;
    float m_friction;     
    float m_jumpVelocity;
    float m_gravity;
    
    // --- THÔNG SỐ RPG ---
    int m_maxHealth;
    int m_currentHealth;
    int m_attackPower;
    int m_defense;

    // ─── dependency ───
    IMapManager* m_mapManager = nullptr;
    bool m_isInvincible = false;
    float m_invincibilityTimer = 0.f;
    static constexpr float INVINCIBILITY_DURATION = 2.0f;

    // ─── tile collision ───
    void tileCollisionX(float deltaTime);
    void tileCollisionY(float deltaTime);

    // --- TRẠNG THÁI HIỆN TẠI ---
    float m_positionX, m_positionY;
    float m_spawnX, m_spawnY;   // vị trí spawn ban đầu (dùng cho respawn)
    float m_velocityX, m_velocityY;
    int   m_score;
    bool  m_isAlive;
    bool  m_isGrounded;
    bool  m_isJumping;
    int   m_facingDirection = 1; // 1 = phải, -1 = trái — hướng nhìn, giữ nguyên khi đứng yên

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

public:
    PlayerManager();
    ~PlayerManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    bool  isAlive()     const override;
    int   getScore()    const override;
    float getPositionX() const override;
    float getPositionY() const override;

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
};
