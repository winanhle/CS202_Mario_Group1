#include "PlayerManager.h"
#include <SFML/Graphics.hpp>
#include "input/Command.h"
#include "input/PlayerInputHandler.h"
#include "forms/FireForm.h"
#include "../../entities/map/MapManager.h"

PlayerManager::PlayerManager()
    : m_score(0), m_isAlive(true), 
      m_positionX(100.0f), m_positionY(150.0f),
      m_spawnX(100.0f),    m_spawnY(150.0f),
      m_velocityX(0),      m_velocityY(0),
      m_playerSprite(m_playerTexture)
{
    // KeyBinding được set từ ngoài qua setKeyBinding() trước khi vào game loop.
    // Xem KeyBindingPresets.h để biết các preset có sẵn.
}

void PlayerManager::initialize()
{
    setupStats();
    m_currentHealth = m_maxHealth;

    if (!m_playerTexture.loadFromFile(getTexturePath())) {
        throw std::runtime_error("Failed to load player sprite sheet: " + getTexturePath());
    }
    m_playerSprite.setTexture(m_playerTexture);

    if (!m_currentForm) {
        throw std::runtime_error("No form set before initialize()");
    }
    m_playerSize = m_currentForm->getHitboxSize();
    m_playerSprite.setTextureRect(m_currentForm->getWalkFrame1());
    m_playerSprite.setOrigin({m_playerSize.x / 2.f, m_playerSize.y / 2.f});

    if (m_fireballManager)
        m_fireballManager->initialize();
}

void PlayerManager::setKeyBinding(const KeyBinding& keys)
{
    m_inputHandler = std::make_unique<PlayerInputHandler>(keys);
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

    if (m_facingDirection < 0) {
        m_playerSprite.setScale({-1.f, 1.f});
    } else if (m_facingDirection > 0) {
        m_playerSprite.setScale({1.f, 1.f});
    }
}

void PlayerManager::update(float deltaTime)
{
    if (!m_isAlive) return;

    // Tick down invincibility i-frames
    if (m_isInvincible) {
        m_invincibilityTimer -= deltaTime;
        if (m_invincibilityTimer <= 0.f) {
            m_isInvincible = false;
            m_invincibilityTimer = 0.f;
        }
    }

    if (m_inputHandler) {
        Command* moveCommand = m_inputHandler->handleRealtimeInput();
        if (moveCommand) moveCommand->execute(*this);
    }

    m_velocityY += m_gravity * deltaTime;

    // Tick cooldown của form hiện tại (FireForm → shoot cooldown)
    if (m_currentForm)
        m_currentForm->update(deltaTime);

    if (m_fireballManager)
        m_fireballManager->update(deltaTime);

    tileCollisionX(deltaTime);
    tileCollisionY(deltaTime);

    updateAnimation(deltaTime);

    m_playerSprite.setPosition({
        m_positionX + m_playerSize.x / 2.f,
        m_positionY + m_playerSize.y / 2.f
    });
}

void PlayerManager::handleInput(const sf::Event& event) {
    if (!m_isAlive || !m_inputHandler) return;

    Command* command = m_inputHandler->handleEvent(event);
    if (command) {
        command->execute(*this);
    }
}

void PlayerManager::render(sf::RenderWindow& window) const
{
    if (!m_isAlive) return;
    window.draw(m_playerSprite);
    if (m_fireballManager)
        m_fireballManager->render(window);
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

float PlayerManager::getPositionX() const
{
    return m_positionX;
}

float PlayerManager::getPositionY() const
{
    return m_positionY;
}

// ==================== COMMAND PATTERN ====================
void JumpCommand::execute(PlayerManager& player)          { player.jump(); }
void StopJumpCommand::execute(PlayerManager& player)      { player.stopJump(); }
void MoveLeftCommand::execute(PlayerManager& player)      { player.moveLeft(); }
void MoveRightCommand::execute(PlayerManager& player)     { player.moveRight(); }
void StopHorizontalCommand::execute(PlayerManager& player){ player.stopHorizontal(); }
void ShootCommand::execute(PlayerManager& player)         { player.shoot(); }

// ==================== DEPENDENCY INJECTION ====================

void PlayerManager::setMapManager(IMapManager* map)
{
    m_mapManager = map;
    if (m_fireballManager)
        m_fireballManager->setMapManager(map);
}

// ==================== DIE / FORM-TYPE ====================

void PlayerManager::die()
{
    m_isAlive = false;
}

FormType PlayerManager::getFormType() const
{
    if (dynamic_cast<FireForm*>(m_currentForm.get()))  return FormType::Fire;
    if (dynamic_cast<SuperForm*>(m_currentForm.get())) return FormType::Super;
    return FormType::Normal;
}

void PlayerManager::setFireballEnemyTarget(IEnemyManager* enemies)
{
    if (m_fireballManager)
        m_fireballManager->setEnemyManager(enemies);
}

// ==================== SHOOT ====================

void PlayerManager::shoot()
{
    // Chỉ bắn nếu đang là FireForm VÀ cooldown đã xong
    if (auto* ff = dynamic_cast<FireForm*>(m_currentForm.get()))
    {
        if (ff->canShoot() && m_fireballManager)
        {
            // Bắn theo hướng nhìn của player (đã được flip sprite)
            int dir = m_facingDirection;
            float spawnX = m_positionX + m_playerSize.x / 2.f;
            float spawnY = m_positionY + m_playerSize.y / 2.f;
            m_fireballManager->spawnFireball(spawnX, spawnY, dir);
            ff->triggerShootCooldown();
        }
    }
}

// ==================== MOVEMENT ====================

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

void PlayerManager::moveLeft()       { m_facingDirection = -1; m_velocityX = -m_maxSpeed; }
void PlayerManager::moveRight()      { m_facingDirection =  1; m_velocityX =  m_maxSpeed; }
void PlayerManager::stopHorizontal() { m_velocityX =  0; }

// ==================== HITBOX ====================

sf::FloatRect PlayerManager::getHitbox() const {
    return sf::FloatRect({m_positionX, m_positionY}, m_playerSize);
}

// ==================== TILE COLLISION ====================

void PlayerManager::tileCollisionX(float deltaTime) {
    if (!m_mapManager) return;
    float tileSize = static_cast<float>(m_mapManager->getTileSize());
    float newX = m_positionX + m_velocityX * deltaTime;

    float left   = newX;
    float right  = newX + m_playerSize.x - 0.01f;
    float top    = m_positionY;
    float bottom = m_positionY + m_playerSize.y - 0.01f;

    int gridX_min = static_cast<int>(left   / tileSize);
    int gridX_max = static_cast<int>(right  / tileSize);
    int gridY_min = static_cast<int>(top    / tileSize);
    int gridY_max = static_cast<int>(bottom / tileSize);

    bool collided = false;
    for (int gy = gridY_min; gy <= gridY_max; ++gy) {
        for (int gx = gridX_min; gx <= gridX_max; ++gx) {
            float probeX = gx * tileSize + 1.0f;
            float probeY = gy * tileSize + 1.0f;

            // Check DEATH_ZONE on the side
            MapManager* mm = dynamic_cast<MapManager*>(m_mapManager);
            if (mm) {
                TileType sideType = mm->getTileType(probeX, probeY);
                if (sideType == TileType::DEATH_ZONE) {
                    die();
                    m_positionX = newX;
                    return;
                }
            }

            if (m_mapManager->isSolid(probeX, probeY)) {
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

    float left   = m_positionX;
    float right  = m_positionX + m_playerSize.x - 0.01f;
    float top    = newY;
    float bottom = newY + m_playerSize.y - 0.01f;

    int gridX_min = static_cast<int>(left   / tileSize);
    int gridX_max = static_cast<int>(right  / tileSize);
    int gridY_min = static_cast<int>(top    / tileSize);
    int gridY_max = static_cast<int>(bottom / tileSize);

    bool collided = false;
    for (int gy = gridY_min; gy <= gridY_max; ++gy) {
        for (int gx = gridX_min; gx <= gridX_max; ++gx) {
            float probeX = gx * tileSize + 1.0f;
            float probeY = gy * tileSize + 1.0f;

            if (m_velocityY > 0) {
                // ── Falling → chỉ va chạm khi tile solid bình thường (HIDDEN xuyên qua) ──
                if (m_mapManager->isSolid(probeX, probeY)) {
                    newY = gy * tileSize - m_playerSize.y;
                    m_isGrounded = true;
                    m_velocityY = 0;
                    collided = true;
                    break;
                }
            } else if (m_velocityY < 0) {
                // ── Đi lên → đập underside (HIDDEN_BLOCK chỉ bump được ở đây) ─────────────
                if (m_mapManager->isSolidFromBelow(probeX, probeY)) {
                    newY = (gy + 1) * tileSize;
                    // Fire tile interaction
                    m_mapManager->onHitFromBelow(gx, gy, static_cast<int>(getFormType()));
                    m_velocityY = 0;
                    collided = true;
                    break;
                }
            }
        }
        if (collided) break;
    }
    m_positionY = newY;

    // ── DEATH_ZONE scan: any tile overlapping player's body kills instantly ──
    {
        MapManager* mm = dynamic_cast<MapManager*>(m_mapManager);
        if (mm && m_isAlive) {
            // Sample the four corners of the final hitbox
            float r = m_positionX + m_playerSize.x - 0.01f;
            float b = m_positionY + m_playerSize.y - 0.01f;
            float cx = m_positionX + m_playerSize.x * 0.5f;
            const float checkPoints[4][2] = {
                { m_positionX + 1.f, m_positionY + 1.f },
                { r, m_positionY + 1.f },
                { m_positionX + 1.f, b },
                { r, b }
            };
            for (auto& pt : checkPoints) {
                TileType t = mm->getTileType(pt[0], pt[1]);
                if (t == TileType::DEATH_ZONE) {
                    die();
                    break;
                }
            }
        }
    }
}

// ==================== BEHAVIOR ====================

void PlayerManager::resetToStart()
{
    m_positionX = m_spawnX;
    m_positionY = m_spawnY;
    m_velocityX = 0.f;
    m_velocityY = 0.f;
    m_isGrounded = false;
}

void PlayerManager::respawn()
{
    resetToStart();
    m_isAlive    = true;
    m_isInvincible      = true;
    m_invincibilityTimer = INVINCIBILITY_DURATION;
}

void PlayerManager::takeDamage()
{
    if (m_isInvincible) return;

    // Cố gắng demote form (Fire→Super, Super→Normal)
    if (auto newForm = m_currentForm->takeDamage())
    {
        setForm(std::move(newForm));
        m_isInvincible       = true;
        m_invincibilityTimer = INVINCIBILITY_DURATION;
    }
    else
    {
        // NormalForm → chết, GameWorld sẽ xử lý shared lives
        m_isAlive = false;
    }
}

void PlayerManager::bounce() {
    m_velocityY  = m_jumpVelocity * 0.7f;
    m_isGrounded = false;
}

void PlayerManager::collectCoin(int amount) {
    m_score += amount;
}
