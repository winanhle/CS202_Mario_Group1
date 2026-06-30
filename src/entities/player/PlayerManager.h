#pragma once
#include <SFML/Graphics.hpp>
#include "../../interfaces/IPlayerManager.h"
#include "../../interfaces/IMapManager.h"
#include "input/PlayerInputHandler.h"
#include "states/PlayerState.h"
#include <memory>

/**
 * @class PlayerManager
 * @brief Stub implementation of player management
 * 
 * Developer: Le Tran
 * Status: STUB - Replace with full implementation
 * 
 * This stub provides minimal functionality to keep the project compilable.
 * Le Tran will implement full Mario character logic here.
 */

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

    // ─── MỚI: dependency ───
    IMapManager* m_mapManager = nullptr;
    bool m_isInvincible = false;

    // ─── MỚI: tile collision — âm thầm bên trong ───
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
    std::unique_ptr<PlayerState> m_currentState;
    std::unique_ptr<PlayerInputHandler> m_inputHandler;

    // Hàm thuần ảo bắt buộc lớp con phải tự định nghĩa thông số
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

    void changeState(std::unique_ptr<PlayerState> newState);
    void jump();
    void stopJump();
    void stopHorizontal();
    void moveLeft();
    void moveRight();

    // ─── MỚI: override ───
    sf::FloatRect getHitbox() const override;
    void takeDamage() override;
    void bounce() override;
    void collectCoin(int amount) override;
    void setMapManager(IMapManager* map) override { m_mapManager = map; }
};
