#pragma once
#include <SFML/Graphics.hpp>
#include "../../interfaces/IPlayerManager.h"
#include "../../interfaces/IMapManager.h"
#include "input/PlayerInputHandler.h"
#include "forms/IPlayerForm.h"
#include <memory>

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
    static constexpr float INVINCIBILITY_DURATION = 2.0f; // seconds of i-frames after a hit

    // ─── tile collision ───
    void tileCollisionX(float deltaTime);
    void tileCollisionY(float deltaTime);

    // --- TRẠNG THÁI HIỆN TẠI ---
    float m_positionX, m_positionY;
    float m_velocityX, m_velocityY;
    int m_score;
    int m_lives;
    bool m_isAlive;
    bool m_isGrounded;
    bool m_isJumping;

    // --- PATTERNS ---
    std::unique_ptr<PlayerInputHandler> m_inputHandler;
    std::unique_ptr<IPlayerForm> m_currentForm;

    // --- SPRITE & ANIMATION ---
    sf::Texture m_playerTexture;
    sf::Sprite m_playerSprite;
    sf::Vector2f m_playerSize;
    int m_currentFrame = 0;
    float m_animTimer = 0.f;

    void updateAnimation(float deltaTime);
    void resetToStart();   // respawn at starting position

    virtual void setupStats() = 0;

public:
    PlayerManager();
    ~PlayerManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    bool isAlive() const override;
    int getScore() const override;
    int getLives() const override;
    float getPositionX() const override;
    float getPositionY() const override;

    void jump();
    void stopJump();
    void stopHorizontal();
    void moveLeft();
    void moveRight();

    void setForm(std::unique_ptr<IPlayerForm> newForm);
    void collectPowerUp(int type) override;

    sf::FloatRect getHitbox() const override;
    void takeDamage() override;
    void bounce() override;
    void collectCoin(int amount) override;
    void setMapManager(IMapManager* map) override { m_mapManager = map; }
};
