#include "PlayerManager.h"
#include <SFML/Graphics.hpp>
#include "input/Command.h"
#include "input/PlayerInputHandler.h"
#include "../../interfaces/ISettingsManager.h"
#include "../../interfaces/ISoundManager.h"
#include "forms/FireForm.h"
#include "../../entities/map/MapManager.h"
#include <algorithm>
#include <cmath>

PlayerManager::PlayerManager()
    : m_score(0), m_isAlive(true), 
      m_positionX(100.0f), m_positionY(150.0f),
      m_spawnX(100.0f),    m_spawnY(150.0f),
      m_velocityX(0),      m_velocityY(0),
      m_playerSprite(m_playerTexture)
{
    // Key bindings are built in initialize() from ISettingsManager
    // (or defaults) so settings-menu rebinds take effect.
    // KeyBinding can also be set from outside via setKeyBinding() before the game loop.
}

void PlayerManager::initialize(ISettingsManager* settings)
{
    rebuildKeyBindings(settings);

    if (m_isInitialized)
        return;

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
    const sf::IntRect firstRect = m_currentForm->getWalkFrame1();
    m_playerSprite.setTextureRect(firstRect);
    m_playerSprite.setOrigin({static_cast<float>(firstRect.size.x) / 2.f, static_cast<float>(firstRect.size.y) / 2.f});
    m_isInitialized = true;

    if (m_fireballManager)
        m_fireballManager->initialize();
}

void PlayerManager::setPlayerIndex(int index)
{
    m_playerIndex = index;
}

void PlayerManager::setTwoPlayerMode(bool isTwoPlayer)
{
    m_isTwoPlayerMode = isTwoPlayer;
}

void PlayerManager::setKeyBinding(const KeyBinding& keys)
{
    m_inputHandler = std::make_unique<PlayerInputHandler>(keys);
}

void PlayerManager::rebuildKeyBindings(ISettingsManager* settings)
{
    KeyBinding keys;
    keys.jump1st  = sf::Keyboard::Key::Unknown;
    keys.jump2nd  = sf::Keyboard::Key::Unknown;
    keys.jump3rd  = sf::Keyboard::Key::Unknown;
    keys.left1st  = sf::Keyboard::Key::Unknown;
    keys.left2nd  = sf::Keyboard::Key::Unknown;
    keys.right1st = sf::Keyboard::Key::Unknown;
    keys.right2nd = sf::Keyboard::Key::Unknown;
    keys.down1st  = sf::Keyboard::Key::Unknown;
    keys.down2nd  = sf::Keyboard::Key::Unknown;
    keys.shoot    = sf::Keyboard::Key::Unknown;
    keys.run      = sf::Keyboard::Key::Unknown;

    if (m_playerIndex == 1)
    {
        // Player 1 strictly uses P1 keybindings (default: Space, A, D, F)
        keys.jump1st  = settings ? settings->getKey(GameAction::P1Jump)      : sf::Keyboard::Key::Space;
        keys.left1st  = settings ? settings->getKey(GameAction::P1MoveLeft)  : sf::Keyboard::Key::A;
        keys.right1st = settings ? settings->getKey(GameAction::P1MoveRight) : sf::Keyboard::Key::D;
        keys.down1st  = sf::Keyboard::Key::S;
        keys.shoot    = settings ? settings->getKey(GameAction::P1Shoot)     : sf::Keyboard::Key::F;
        keys.run      = sf::Keyboard::Key::LShift;

        // Default jump keys: both Space (primary) and W (secondary) work by default in both 1P and 2P modes.
        // If the player customizes P1 Jump in Settings (e.g. to J, K, etc.), the W fallback is disabled.
        if (keys.jump1st == sf::Keyboard::Key::Space) {
            keys.jump2nd = sf::Keyboard::Key::W;
        }

        // Arrow key fallbacks for P1 are active ONLY in single-player mode.
        // In 2-Player mode, Player 1 must NOT capture Arrow keys since they belong to Player 2.
        if (!m_isTwoPlayerMode)
        {
            if (keys.jump1st == sf::Keyboard::Key::Space) {
                keys.jump3rd = sf::Keyboard::Key::Up;
            }
            if (keys.left1st == sf::Keyboard::Key::A) {
                keys.left2nd = sf::Keyboard::Key::Left;
            }
            if (keys.right1st == sf::Keyboard::Key::D) {
                keys.right2nd = sf::Keyboard::Key::Right;
            }
            keys.down2nd = sf::Keyboard::Key::Down;
        }

        // Conflict guard: Clear any secondary/tertiary key if it conflicts with ANY primary key of another action
        auto isConflictWithPrimary = [&](sf::Keyboard::Key k, GameAction ownAction) {
            if (k == sf::Keyboard::Key::Unknown) return false;
            if (ownAction != GameAction::P1Jump && k == keys.jump1st) return true;
            if (ownAction != GameAction::P1MoveLeft && k == keys.left1st) return true;
            if (ownAction != GameAction::P1MoveRight && k == keys.right1st) return true;
            if (k == keys.shoot) return true;
            return false;
        };

        if (isConflictWithPrimary(keys.jump2nd, GameAction::P1Jump) || keys.jump2nd == keys.jump1st)
            keys.jump2nd = sf::Keyboard::Key::Unknown;
        if (isConflictWithPrimary(keys.jump3rd, GameAction::P1Jump) || keys.jump3rd == keys.jump1st || keys.jump3rd == keys.jump2nd)
            keys.jump3rd = sf::Keyboard::Key::Unknown;

        if (isConflictWithPrimary(keys.left2nd, GameAction::P1MoveLeft) || keys.left2nd == keys.left1st)
            keys.left2nd = sf::Keyboard::Key::Unknown;

        if (isConflictWithPrimary(keys.right2nd, GameAction::P1MoveRight) || keys.right2nd == keys.right1st)
            keys.right2nd = sf::Keyboard::Key::Unknown;
    }
    else if (m_playerIndex == 2)
    {
        // Player 2 strictly uses P2 keybindings (default: Up, Left, Right, Period)
        keys.jump1st  = settings ? settings->getKey(GameAction::P2Jump)      : sf::Keyboard::Key::Up;
        keys.left1st  = settings ? settings->getKey(GameAction::P2MoveLeft)  : sf::Keyboard::Key::Left;
        keys.right1st = settings ? settings->getKey(GameAction::P2MoveRight) : sf::Keyboard::Key::Right;
        keys.down1st  = sf::Keyboard::Key::Down;
        keys.shoot    = settings ? settings->getKey(GameAction::P2Shoot)     : sf::Keyboard::Key::Period;
        keys.run      = sf::Keyboard::Key::RShift;
    }

    m_inputHandler = std::make_unique<PlayerInputHandler>(keys);
}


void PlayerManager::updateAnimation(float deltaTime)
{
    const float FRAME_TIME = 0.15f;

    sf::IntRect currentRect;
    if (m_isFlagpoleSliding) {
        // While sliding down and on touchdown: use clean standard in-game sprite
        currentRect = m_currentForm->getWalkFrame1();
    } else if (!m_isGrounded) {
        currentRect = m_currentForm->getJumpFrame();
    } else if (std::abs(m_velocityX) > 0.f && m_isSkidding) {
        // SKID: đang phanh gấp → đứng hình frame (chưa có sprite skid riêng,
        // dùng frame đứng yên + sprite đã flip theo hướng bấm ngược).
        currentRect = m_currentForm->getWalkFrame1();
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

    if (m_isPipeTraveling) {
        const float destinationX = m_isPipeAligning ? m_pipeAlignX : m_pipeTargetX;
        const float destinationY = m_isPipeAligning ? m_pipeAlignY : m_pipeTargetY;
        const float dx = destinationX - m_positionX;
        const float dy = destinationY - m_positionY;
        const float distance = std::sqrt(dx * dx + dy * dy);
        const float speed = m_isPipeAligning ? PIPE_ALIGN_SPEED : PIPE_TRAVEL_SPEED;
        const float step = speed * deltaTime;

        if (distance <= step || distance <= 0.01f) {
            m_positionX = destinationX;
            m_positionY = destinationY;
            if (m_isPipeAligning) {
                m_isPipeAligning = false;
            } else {
                m_isPipeTraveling = false;
            }
        } else {
            m_positionX += dx / distance * step;
            m_positionY += dy / distance * step;
        }

        updateAnimation(deltaTime);
        m_playerSprite.setPosition({
            m_positionX + m_playerSize.x / 2.f,
            m_positionY + m_playerSize.y / 2.f
        });
        return;
    }

    if (m_isFlagpoleSliding) {
        if (!m_isGrounded) {
            m_velocityY = 60.f; // Smooth, slow downward glide from contact height
            m_positionY += m_velocityY * deltaTime;
            tileCollisionY(deltaTime);
        } else {
            m_velocityY = 0.f;
            m_flagpoleFinishTimer += deltaTime;
            if (m_flagpoleFinishTimer >= 0.8f) {
                m_isFlagpoleSliding = false;
                m_hasFinishedFlagpole = true;
            }
        }
        updateAnimation(deltaTime);
        m_playerSprite.setPosition({
            m_positionX + m_playerSize.x / 2.f,
            m_positionY + m_playerSize.y / 2.f
        });
        return;
    }

    // Tick down invincibility i-frames
    if (m_isInvincible) {
        m_invincibilityTimer -= deltaTime;
        if (m_invincibilityTimer <= 0.f) {
            m_isInvincible = false;
            m_invincibilityTimer = 0.f;
        }
    }

    // Tick StarState; xoá khi hết thời gian
    if (m_starState) {
        m_starState->update(deltaTime);
        if (!m_starState->isActive())
            m_starState.reset();
    }

    if (m_inputHandler) {
        Command* moveCommand = m_inputHandler->handleRealtimeInput();
        if (moveCommand) moveCommand->execute(*this);
    }

    // Vật lý ngang mỗi frame (tăng tốc / friction / động lượng trên không)
    applyHorizontalPhysics(deltaTime);

    // Trọng lực = base; nếu ĐANG GIỮ nút nhảy lúc đà đi lên (và còn trong cửa sổ
    // m_jumpHoldGrace) → giảm trọng lực → người nhảy cao hơn. Hết thời gian giữ
    // hoặc đã qua đỉnh (vY >= 0) thì trọng lực trở về bình thường → độ cao bị chặn.
    float gravity = m_gravity;
    if (m_isJumping && m_inputHandler && m_inputHandler->isJumpKeyHeld() &&
        m_velocityY < 0.f && m_jumpHoldTimer < m_jumpHoldGrace) {
        gravity *= (1.f - m_jumpHoldBoost);
        m_jumpHoldTimer += deltaTime;
        if (m_jumpHoldTimer > m_jumpHoldGrace) m_jumpHoldTimer = m_jumpHoldGrace;

        if (m_jumpHoldTimer >= 0.08f && !m_jumpSoundPlayedBig) {
            m_jumpSoundPlayedBig = true;
            if (m_soundManager)
                m_soundManager->playJump();
        }
    }
    m_velocityY += gravity * deltaTime;

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
    if (!m_isAlive || !m_inputHandler || m_isFlagpoleSliding || m_isPipeTraveling) return;

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
    // Vẽ hiệu ứng chop nháy 4 màu khi đang Star state
    if (m_starState)
        m_starState->render(window, getHitbox());
}

void PlayerManager::setForm(std::unique_ptr<IPlayerForm> newForm)
{
    float oldHeight = m_playerSize.y;
    float newHeight = newForm->getHitboxSize().y;
    m_positionY -= (newHeight - oldHeight);
    m_playerSize = newForm->getHitboxSize();
    m_currentForm = std::move(newForm);

    // Áp dụng ngay frame + origin của form mới, tránh sprite bị lệch
    // (dùng rect cũ) trong 1 frame ngay sau khi đổi dạng.
    sf::IntRect rect = m_currentForm->getWalkFrame1();
    m_playerSprite.setTextureRect(rect);
    m_playerSprite.setOrigin({(float)rect.size.x / 2.f, (float)rect.size.y / 2.f});
}

void PlayerManager::collectPowerUp(int type)
{
    auto t = static_cast<PowerUpType>(type);
    FormType prevForm = getFormType();
    if (auto newForm = m_currentForm->evolve(t)) {
        setForm(std::move(newForm));
    }
    FormType newForm = getFormType();

    if (m_powerUpCallback && newForm != prevForm)
        m_powerUpCallback(newForm);

    if (m_soundManager)
        m_soundManager->playItem();
}

bool PlayerManager::isAlive() const
{
    return m_isAlive;
}

int PlayerManager::getScore() const
{
    return m_score;
}


void PlayerManager::restoreState(int score, int coins)
{
    m_score = score;
    m_coins = coins;
    m_isAlive = true;
}

void PlayerManager::setSpawnPoint(float x, float y)
{
    m_spawnX = x;
    m_spawnY = y;
    m_positionX = x;
    m_positionY = y;
    m_velocityX = 0.f;
    m_velocityY = 0.f;
}
float PlayerManager::getPositionX() const
{
    return m_positionX;
}

float PlayerManager::getPositionY() const
{
    return m_positionY;
}

bool PlayerManager::isDownPressed() const
{
    return m_inputHandler && m_inputHandler->isDownKeyHeld();
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

void PlayerManager::setSoundManager(ISoundManager* sound)
{
    m_soundManager = sound;
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
            m_fireballManager->spawnFireball(spawnX, spawnY, dir, this);
            ff->triggerShootCooldown();

            if (m_soundManager)
                m_soundManager->playFireball();
        }
    }
}

// ==================== MOVEMENT ====================

void PlayerManager::jump() {
    if (m_isGrounded) {
        m_velocityY = m_jumpVelocity;
        m_isGrounded = false;
        m_isJumping = true;
        m_jumpHoldTimer = 0.f; // bắt đầu cửa sổ giữ nút để nhảy cao hơn
        m_jumpSoundPlayedBig = false;

        if (m_soundManager) {
            if (getFormType() != FormType::Normal) {
                m_soundManager->playJump();
                m_jumpSoundPlayedBig = true;
            } else {
                m_soundManager->playJumpSmall();
            }
        }
    }
}

void PlayerManager::stopJump() {
    if (m_isJumping && m_velocityY < 0.0f) {
        m_velocityY *= 0.5f; 
        m_isJumping = false;
    }
}

// Các hàm nhập chỉ GHI hướng phím (m_inputDirection); tốc độ do
// applyHorizontalPhysics tính mỗi frame — không còn đặt vận tốc tức thời.
void PlayerManager::moveLeft() {
    m_inputDirection = -1;
    if (m_isGrounded) m_facingDirection = -1;
}
void PlayerManager::moveRight() {
    m_inputDirection = 1;
    if (m_isGrounded) m_facingDirection = 1;
}
void PlayerManager::stopHorizontal() { m_inputDirection = 0; }

void PlayerManager::applyHorizontalPhysics(float deltaTime) {
    m_isSkidding = false; // reset mỗi frame, set lại nếu đang phanh gấp

    const int  input  = m_inputDirection;
    const bool running = m_inputHandler && m_inputHandler->isRunKeyHeld();
    // Max speed kép: giữ phím chạy → RUN, ngược lại → WALK (gia tốc như nhau)
    const float currentMax = running ? m_runMaxSpeed : m_maxSpeed;
    float v = m_velocityX;

    if (m_isGrounded) {
        if (input != 0) {
            if (v * input < 0.f) {
                // ── SKID: bấm ngược hướng di chuyển → phanh cực mạnh ──
                m_isSkidding = true;
                v += static_cast<float>(input) * m_skidDeceleration * deltaTime;
                if (v * input > 0.f) v = 0.f; // không bật ngược hướng trong cùng frame
            } else {
                // ── Cùng hướng → tăng tốc tuyến tính tới currentMax ──
                v += static_cast<float>(input) * m_acceleration * deltaTime;
                if (v * input > currentMax)
                    v = static_cast<float>(input) * currentMax;
            }
        } else {
            // ── Thả phím → ma sát TUYẾN TÍNH (hằng số) về 0, không bị giật lùi ──
            if (v > 0.f)      v = std::max(0.f, v - m_deceleration * deltaTime);
            else if (v < 0.f) v = std::min(0.f, v + m_deceleration * deltaTime);
        }
    } else {
        // ── TRÊN KHÔNG: giữ quán tính lúc rời đất, air control RẤT yếu ──
        if (input != 0) {
            v += static_cast<float>(input) * m_airAcceleration * deltaTime;
            if (v >  currentMax) v =  currentMax;
            if (v < -currentMax) v = -currentMax;
        }
        // Không bấm gì → giữ nguyên vận tốc ngang (NES không có air friction)
    }

    m_velocityX = v;

    // Hướng nhìn: LUÔN theo phím bấm ở mọi trạng thái (kể cả trên không).
    // Quán tính (m_velocityX) để vật lý xử lý, sprite quay đầu ngay lập tức
    // → player luôn cảm thấy kiểm soát tuyệt đối (chuẩn Mario feel).
    if (input != 0) m_facingDirection = input;
}

// ==================== HITBOX ====================

sf::FloatRect PlayerManager::getHitbox() const {
    return sf::FloatRect({m_positionX, m_positionY}, m_playerSize);
}

// ==================== TILE COLLISION ====================

void PlayerManager::tileCollisionX(float deltaTime) {
    float newX = m_positionX + m_velocityX * deltaTime;

    if (m_mapManager) {
        float tileSize = static_cast<float>(m_mapManager->getTileSize());
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
                    m_mapManager->onSideTouch(gx, gy, this);
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
    }

    // ── Lift platform horizontal side collision ──
    if (m_liftManager) {
        m_liftManager->resolveCollisionX(m_positionX, newX, m_positionY, m_playerSize, m_velocityX);
    }

    m_positionX = newX;
}

void PlayerManager::tileCollisionY(float deltaTime) {
    float newY = m_positionY + m_velocityY * deltaTime;

    m_isGrounded = false;

    if (m_mapManager) {
        float tileSize = static_cast<float>(m_mapManager->getTileSize());
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
                        m_mapManager->onStandingOn(gx, gy, this);
                        m_velocityY = 0;
                        collided = true;
                        break;
                    }
                } else if (m_velocityY < 0) {
                    // ── Đi lên → đập underside (HIDDEN_BLOCK chỉ bump được ở đây) ─────────────
                    if (m_mapManager->isSolidFromBelow(probeX, probeY)) {
                        newY = (gy + 1) * tileSize;
                        // Fire tile interaction
                        m_mapManager->onHitFromBelow(gx, gy, this);
                        m_velocityY = 0;
                        collided = true;
                        break;
                    }
                }
            }
            if (collided) break;
        }
    }

    // ── Lift platform vertical collision (landing, riding, head bump) ──
    if (m_liftManager) {
        m_liftManager->resolveCollisionY(m_positionX, m_positionY, newY, m_playerSize, m_velocityY, m_isGrounded, m_isJumping);
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
    m_inputDirection = 0;
    m_isGrounded = false;
    m_isPipeTraveling = false;
    m_isPipeAligning = false;
    m_pipeAlignX = m_positionX;
    m_pipeAlignY = m_positionY;
    m_pipeTargetX = m_positionX;
    m_pipeTargetY = m_positionY;
    m_isFlagpoleSliding = false;
    m_hasFinishedFlagpole = false;
    m_flagpoleFinishTimer = 0.f;
}

void PlayerManager::startPipeTravel(float alignX, float alignY, float targetX, float targetY)
{
    if (!m_isAlive || m_isPipeTraveling)
        return;

    m_velocityX = 0.f;
    m_velocityY = 0.f;
    m_inputDirection = 0;
    m_isGrounded = false;
    m_pipeAlignX = alignX;
    m_pipeAlignY = alignY;
    m_pipeTargetX = targetX;
    m_pipeTargetY = targetY;
    m_isPipeAligning = std::abs(m_positionX - alignX) > 0.01f ||
                        std::abs(m_positionY - alignY) > 0.01f;
    m_isPipeTraveling = true;
}

void PlayerManager::startFlagpoleSlide(float poleX)
{
    if (m_isFlagpoleSliding || m_hasFinishedFlagpole)
        return;

    m_isFlagpoleSliding = true;
    m_hasFinishedFlagpole = false;

    if (m_soundManager)
        m_soundManager->playFlagpole();
    m_isGrounded = false; // Start sliding down from current air position
    m_flagpoleFinishTimer = 0.f;
    m_positionX = poleX - 4.f; // Align character nicely with the flagpole column
    // m_positionY is preserved at current contact height!
    m_velocityX = 0.f;
    m_velocityY = 60.f; // Slow, smooth drop
    m_facingDirection = 1;
    m_inputDirection = 0;
}

void PlayerManager::respawn()
{
    resetToStart();
    m_isAlive    = true;
    m_isInvincible      = true;
    m_invincibilityTimer = INVINCIBILITY_DURATION;
    m_starState.reset();
    if (m_fireballManager)
    {
        m_fireballManager->clear();
    }
}

void PlayerManager::takeDamage()
{
    // Star state → bất tử, bỏ qua hoàn toàn
    if (m_starState && m_starState->isActive()) return;

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

// ==================== STAR ====================

void PlayerManager::activateStar()
{
    m_starState = std::make_unique<StarState>();
    if (m_starCallback) m_starCallback();
    if (m_soundManager)
        m_soundManager->playItem();
}

bool PlayerManager::isStarActive() const
{
    return m_starState && m_starState->isActive();
}

void PlayerManager::bounce() {
    m_velocityY  = m_jumpVelocity * 0.7f;
    m_isGrounded = false;
}

void PlayerManager::addScore(int points) {
    m_score += points;
}

void PlayerManager::collectCoin(int amount) {
    m_coins += amount;
    m_score += amount * 200;

    if (m_coinCallback) m_coinCallback(amount);
    if (m_soundManager)
        m_soundManager->playCoin();
    while (m_coins >= 100) {
        m_coins -= 100;
        m_pendingOneUps++;
    }
}

int PlayerManager::consumePendingOneUps() {
    int count = m_pendingOneUps;
    m_pendingOneUps = 0;
    return count;
}

// ==================== LIFT RIDING ====================

void PlayerManager::applyLiftOffset(float dx, float dy)
{
    m_positionX += dx;
    m_positionY += dy;

    // Only force grounded if player is NOT already jumping.
    // If the player just pressed jump, let their jump state play out so they
    // can leave the lift and jump normally.
    if (!m_isJumping) {
        // While riding a lift the player must be treated as grounded so that:
        //   (a) jump() is available
        //   (b) gravity doesn't fight an upward-moving platform
        m_isGrounded = true;
    }
    // Only zero vertical velocity if player is not jumping,
    // otherwise preserve the jump velocity so the player can leave the lift.
    if (!m_isJumping) m_velocityY = 0.f;

    // Keep the sprite position in sync
    m_playerSprite.setPosition({
        m_positionX + m_playerSize.x / 2.f,
        m_positionY + m_playerSize.y / 2.f
    });
}
