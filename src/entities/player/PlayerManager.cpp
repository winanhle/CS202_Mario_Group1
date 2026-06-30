#include "PlayerManager.h"
#include <SFML/Graphics.hpp>
#include "input/Command.h"
#include "input/PlayerInputHandler.h"

PlayerManager::PlayerManager()
    : m_score(0), m_lives(3), m_isAlive(true), 
      m_positionX(100.0f), m_positionY(150.0f),
      m_velocityX(0), m_velocityY(0)
{
    KeyBinding marioKeys;

    marioKeys.jump1st = sf::Keyboard::Key::W;
    marioKeys.jump2nd = sf::Keyboard::Key::Up;
    marioKeys.jump3rd = sf::Keyboard::Key::Space;
    marioKeys.left1st = sf::Keyboard::Key::A;
    marioKeys.left2nd = sf::Keyboard::Key::Left;
    marioKeys.right1st = sf::Keyboard::Key::D;
    marioKeys.right2nd = sf::Keyboard::Key::Right;
    
    m_inputHandler = std::make_unique<PlayerInputHandler>(marioKeys);
}

void PlayerManager::initialize()
{
    setupStats();
    m_currentHealth = m_maxHealth;
}

void PlayerManager::update(float deltaTime)
{
    if (m_inputHandler) {
        Command* moveCommand = m_inputHandler->handleRealtimeInput();
        if (moveCommand) moveCommand->execute(*this);
    }

    if (m_currentState) {
        m_currentState->update(*this, deltaTime);
    }

    m_velocityY += m_gravity * deltaTime;

    // ─── TILE COLLISION ───
    tileCollisionX(deltaTime);
    tileCollisionY(deltaTime);
}

void PlayerManager::handleInput(const sf::Event& event) {
    if (!m_inputHandler) return;

    // Command Pattern cho các phím đã config
    Command* command = m_inputHandler->handleEvent(event);
    if (command) {
        command->execute(*this);
    }

    // State Pattern
    if (m_currentState) {
        m_currentState->handleInput(*this, event);
    }
}

void PlayerManager::changeState(std::unique_ptr<PlayerState> newState) {
    m_currentState = std::move(newState);
    m_currentState->enter(*this);
}

void PlayerManager::render(sf::RenderWindow& window) const
{
    // TODO: Le Tran - Render player
    // For now, draw a placeholder rectangle
    sf::RectangleShape player({ 16.0f, 16.0f });
    player.setFillColor(sf::Color::Red);
    player.setPosition({ m_positionX, m_positionY });
    window.draw(player);
}

bool PlayerManager::isAlive() const
{
    return m_isAlive;
}

int PlayerManager::getScore() const
{
    return m_score;
}

int PlayerManager::getLives() const
{
    return m_lives;
}

float PlayerManager::getPositionX() const
{
    return m_positionX;
}

float PlayerManager::getPositionY() const
{
    return m_positionY;
}

// ==================== COMMAND PATTERN ====================
void JumpCommand::execute(PlayerManager& player) { player.jump(); }
void StopJumpCommand::execute(PlayerManager& player) { player.stopJump(); }
void MoveLeftCommand::execute(PlayerManager& player) { player.moveLeft(); }
void MoveRightCommand::execute(PlayerManager& player) { player.moveRight(); }
void StopHorizontalCommand::execute(PlayerManager& player) { player.stopHorizontal(); }

// ==================== EDGE CASES ====================

void PlayerManager::jump() {
    // Double Jump Prevention
    if (m_isGrounded) {
        m_velocityY = m_jumpVelocity; // Gán lực nhảy (số âm)
        m_isGrounded = false;
        m_isJumping = true;
    }
}

void PlayerManager::stopJump() {
    // Variable Jump Height
    if (m_isJumping && m_velocityY < 0.0f) {
        m_velocityY *= 0.5f; 
        m_isJumping = false;
    }
}

void PlayerManager::moveLeft() { m_velocityX = -m_maxSpeed; }
void PlayerManager::moveRight() { m_velocityX = m_maxSpeed; }
void PlayerManager::stopHorizontal() { m_velocityX = 0; }

// ==================== HITBOX ====================

sf::FloatRect PlayerManager::getHitbox() const {
    return sf::FloatRect({m_positionX, m_positionY}, {16.f, 16.f});
}

// ==================== TILE COLLISION ====================

void PlayerManager::tileCollisionX(float deltaTime) {
    if (!m_mapManager) return;
    float tileSize = static_cast<float>(m_mapManager->getTileSize());
    float newX = m_positionX + m_velocityX * deltaTime;

    // TRỪ EPSILON ĐỂ TRÁNH LẸM VIỀN
    float left = newX;
    float right = newX + 16.f - 0.01f;
    float top = m_positionY;
    float bottom = m_positionY + 16.f - 0.01f;

    int gridX_min = static_cast<int>(left / tileSize);
    int gridX_max = static_cast<int>(right / tileSize);
    int gridY_min = static_cast<int>(top / tileSize);
    int gridY_max = static_cast<int>(bottom / tileSize);

    bool collided = false;
    for (int gy = gridY_min; gy <= gridY_max; ++gy) {
        for (int gx = gridX_min; gx <= gridX_max; ++gx) {
            // Cộng thêm 1.0f để đảm bảo query ĐÚNG vào giữa ô gạch, tránh lỗi float
            if (m_mapManager->isSolid(gx * tileSize + 1.0f, gy * tileSize + 1.0f)) {
                if (m_velocityX > 0) {
                    newX = gx * tileSize - 16.f; // Đụng bên phải, dội lùi lại
                } else if (m_velocityX < 0) {
                    newX = (gx + 1) * tileSize;  // Đụng bên trái, dội tiến lên
                }
                m_velocityX = 0;
                collided = true;
                break; // Xử lý xong va chạm X thì thoát vòng lặp con
            }
        }
        if (collided) break; // Thoát vòng lặp ngoài
    }
    m_positionX = newX;
}

void PlayerManager::tileCollisionY(float deltaTime) {
    if (!m_mapManager) return;
    float tileSize = static_cast<float>(m_mapManager->getTileSize());
    float newY = m_positionY + m_velocityY * deltaTime;

    m_isGrounded = false; 

    float left = m_positionX;
    float right = m_positionX + 16.f - 0.01f;
    float top = newY;
    float bottom = newY + 16.f - 0.01f;

    int gridX_min = static_cast<int>(left / tileSize);
    int gridX_max = static_cast<int>(right / tileSize);
    int gridY_min = static_cast<int>(top / tileSize);
    int gridY_max = static_cast<int>(bottom / tileSize);

    bool collided = false;
    for (int gy = gridY_min; gy <= gridY_max; ++gy) {
        for (int gx = gridX_min; gx <= gridX_max; ++gx) {
            if (m_mapManager->isSolid(gx * tileSize + 1.0f, gy * tileSize + 1.0f)) {
                if (m_velocityY > 0) {
                    newY = gy * tileSize - 16.f; // Chạm đất
                    m_isGrounded = true;         // Cấp lại quyền nhảy
                } else if (m_velocityY < 0) {
                    newY = (gy + 1) * tileSize;  // Đụng trần nhà
                }
                m_velocityY = 0; // Triệt tiêu lực rơi/nhảy
                collided = true;
                break;
            }
        }
        if (collided) break;
    }
    m_positionY = newY;
}

// ==================== BEHAVIOR ====================

void PlayerManager::takeDamage() {
    if (!m_isInvincible) {
        m_currentHealth -= 10;
        if (m_currentHealth <= 0) {
            m_isAlive = false;
        }
    }
}

void PlayerManager::bounce() {
    m_velocityY = m_jumpVelocity * 0.7f;
    m_isGrounded = false;
}

void PlayerManager::collectCoin(int amount) {
    m_score += amount;
}