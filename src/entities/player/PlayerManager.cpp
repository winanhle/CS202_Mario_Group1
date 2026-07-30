#include "PlayerManager.h"
#include <SFML/Graphics.hpp>
#include "input/Command.h"
#include "input/PlayerInputHandler.h"

PlayerManager::PlayerManager()
    : m_score(0), m_lives(3), m_isAlive(true), 
      m_positionX(100.0f), m_positionY(150.0f),
      m_velocityX(0), m_velocityY(0),
      m_playerSprite(m_playerTexture)
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

    if (!m_playerTexture.loadFromFile("assets/texture/mario.png")) {
        throw std::runtime_error("Failed to load mario sprite sheet.");
    }
    m_playerSprite.setTexture(m_playerTexture);

    if (!m_currentForm) {
        throw std::runtime_error("No form set before initialize()");
    }
    m_playerSize = m_currentForm->getHitboxSize();
    m_playerSprite.setTextureRect(m_currentForm->getWalkFrame1());
    m_playerSprite.setOrigin({m_playerSize.x / 2.f, m_playerSize.y / 2.f});
}

void PlayerManager::updateAnimation(float deltaTime)
{
    const float FRAME_TIME = 0.15f;

    sf::IntRect currentRect;
    if (!m_isGrounded) {
        currentRect = m_currentForm->getJumpFrame();
    } else if (std::abs(m_velocityX) > 0.f) {
        m_animTimer += deltaTime;
        if (m_animTimer >= FRAME_TIME) {
            m_animTimer = 0.f;
            m_currentFrame = (m_currentFrame + 1) % 2;
        }
        currentRect = m_currentFrame == 0
            ? m_currentForm->getWalkFrame1()
            : m_currentForm->getWalkFrame2();
    } else {
        currentRect = m_currentForm->getWalkFrame1();
    }
    m_playerSprite.setTextureRect(currentRect);
    m_playerSprite.setOrigin({(float)currentRect.size.x / 2.f, (float)currentRect.size.y / 2.f});

    if (m_velocityX < 0.f) {
        m_playerSprite.setScale({-1.f, 1.f});
    } else if (m_velocityX > 0.f) {
        m_playerSprite.setScale({1.f, 1.f});
    }
}

void PlayerManager::update(float deltaTime)
{
    if (m_inputHandler) {
        Command* moveCommand = m_inputHandler->handleRealtimeInput();
        if (moveCommand) moveCommand->execute(*this);
    }

    m_velocityY += m_gravity * deltaTime;

    tileCollisionX(deltaTime);
    tileCollisionY(deltaTime);

    updateAnimation(deltaTime);

    m_playerSprite.setPosition({
        m_positionX + m_playerSize.x / 2.f,
        m_positionY + m_playerSize.y / 2.f
    });
}

void PlayerManager::handleInput(const sf::Event& event) {
    if (!m_inputHandler) return;

    Command* command = m_inputHandler->handleEvent(event);
    if (command) {
        command->execute(*this);
    }
}

void PlayerManager::render(sf::RenderWindow& window) const
{
    window.draw(m_playerSprite);
}

void PlayerManager::setForm(std::unique_ptr<IPlayerForm> newForm)
{
    float oldHeight = m_playerSize.y;
    float newHeight = newForm->getHitboxSize().y;
    m_positionY -= (newHeight - oldHeight);
    m_playerSize = newForm->getHitboxSize();
    m_currentForm = std::move(newForm);
}

void PlayerManager::collectPowerUp(int type)
{
    auto t = static_cast<PowerUpType>(type);
    if (auto newForm = m_currentForm->evolve(t)) {
        setForm(std::move(newForm));
    }
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
    if (m_isGrounded) {
        m_velocityY = m_jumpVelocity;
        m_isGrounded = false;
        m_isJumping = true;
    }
}

void PlayerManager::stopJump() {
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
    return sf::FloatRect({m_positionX, m_positionY}, m_playerSize);
}

// ==================== TILE COLLISION ====================

void PlayerManager::tileCollisionX(float deltaTime) {
    if (!m_mapManager) return;
    float tileSize = static_cast<float>(m_mapManager->getTileSize());
    float newX = m_positionX + m_velocityX * deltaTime;

    float left = newX;
    float right = newX + m_playerSize.x - 0.01f;
    float top = m_positionY;
    float bottom = m_positionY + m_playerSize.y - 0.01f;

    int gridX_min = static_cast<int>(left / tileSize);
    int gridX_max = static_cast<int>(right / tileSize);
    int gridY_min = static_cast<int>(top / tileSize);
    int gridY_max = static_cast<int>(bottom / tileSize);

    bool collided = false;
    for (int gy = gridY_min; gy <= gridY_max; ++gy) {
        for (int gx = gridX_min; gx <= gridX_max; ++gx) {
            if (m_mapManager->isSolid(gx * tileSize + 1.0f, gy * tileSize + 1.0f)) {
                if (m_velocityX > 0) {
                    newX = gx * tileSize - m_playerSize.x;
                } else if (m_velocityX < 0) {
                    newX = (gx + 1) * tileSize;
                }
                m_velocityX = 0;
                collided = true;
                break;
            }
        }
        if (collided) break;
    }
    m_positionX = newX;
}

void PlayerManager::tileCollisionY(float deltaTime) {
    if (!m_mapManager) return;
    float tileSize = static_cast<float>(m_mapManager->getTileSize());
    float newY = m_positionY + m_velocityY * deltaTime;

    m_isGrounded = false; 

    float left = m_positionX;
    float right = m_positionX + m_playerSize.x - 0.01f;
    float top = newY;
    float bottom = newY + m_playerSize.y - 0.01f;

    int gridX_min = static_cast<int>(left / tileSize);
    int gridX_max = static_cast<int>(right / tileSize);
    int gridY_min = static_cast<int>(top / tileSize);
    int gridY_max = static_cast<int>(bottom / tileSize);

    bool collided = false;
    for (int gy = gridY_min; gy <= gridY_max; ++gy) {
        for (int gx = gridX_min; gx <= gridX_max; ++gx) {
            if (m_mapManager->isSolid(gx * tileSize + 1.0f, gy * tileSize + 1.0f)) {
                if (m_velocityY > 0) {
                    newY = gy * tileSize - m_playerSize.y;
                    m_isGrounded = true;
                } else if (m_velocityY < 0) {
                    newY = (gy + 1) * tileSize;
                }
                m_velocityY = 0;
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
        if (auto newForm = m_currentForm->takeDamage()) {
            setForm(std::move(newForm));
            m_isInvincible = true;
        } else {
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
