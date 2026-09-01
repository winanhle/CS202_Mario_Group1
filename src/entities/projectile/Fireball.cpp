#include "Fireball.h"
#include "FireballSprite.h"
#include "../../interfaces/IMapManager.h"
#include "../../interfaces/IEnemyManager.h"

Fireball::Fireball(float x, float y, int direction, const sf::Texture& texture, IPlayerManager* owner)
    : m_owner(owner),
      m_position(x, y),
      m_velocity(SPEED * static_cast<float>(direction), 0.f),
      m_texture(&texture),
      m_sprite(texture)
{
    m_sprite.setTextureRect(FireballSprite::FlyFrames[0]);
    m_sprite.setOrigin({
        FireballSprite::FlyFrames[0].size.x / 2.f,
        FireballSprite::FlyFrames[0].size.y / 2.f
    });
    m_sprite.setPosition(m_position);
}

void Fireball::update(float dt, const IMapManager& map, IEnemyManager& enemies)
{
    m_lifetime += dt;
    if (m_lifetime > MAX_LIFETIME)
    {
        m_state = FireballState::Dead;
        return;
    }

    switch (m_state)
    {
    case FireballState::Flying:
        applyGravity(dt);
        handleTileCollision(map, dt);
        handleEnemyCollision(enemies);
        updateFlyAnimation(dt);

        // Rơi khỏi map / ra ngoài quá xa → Dead (không qua Exploding)
        {
            const sf::Vector2u mapSize = map.getMapPixelSize();
            if (m_position.y > static_cast<float>(mapSize.y) + 512.f ||
                m_position.x < -1024.f ||
                m_position.x > static_cast<float>(mapSize.x) + 1024.f)
            {
                m_state = FireballState::Dead;
            }
        }
        break;

    case FireballState::Exploding:
        updateExplodeAnimation(dt);
        break;

    case FireballState::Dead:
        break;
    }

    m_sprite.setPosition(m_position);
}

void Fireball::applyGravity(float dt)
{
    m_velocity.y += GRAVITY * dt;
}

void Fireball::handleTileCollision(const IMapManager& map, float dt)
{
    // ── Trục Y (kiểm tra trước) ──
    float newY = m_position.y + m_velocity.y * dt;
    if (collidesWithMap(map, m_position.x, newY))
    {
        if (m_bounceCount < MAX_BOUNCES)
        {
            m_velocity.y = BOUNCE_VY;
            ++m_bounceCount;
            // KHÔNG chuyển state, vẫn Flying
        }
        else
        {
            m_state = FireballState::Exploding; // hết bounce → nổ
            m_currentFrame = 0;
            m_animTimer    = 0.f;
        }
    }
    else
    {
        m_position.y = newY;
    }

    // ── Trục X ──
    float newX = m_position.x + m_velocity.x * dt;
    if (collidesWithMap(map, newX, m_position.y))
    {
        m_state = FireballState::Exploding; // va tường → nổ ngay
        m_currentFrame = 0;
        m_animTimer    = 0.f;
    }
    else
    {
        m_position.x = newX;
    }
}

void Fireball::handleEnemyCollision(IEnemyManager& enemies)
{
    if (enemies.takeDamageFromFireball(getHitbox(), m_owner))
    {
        m_state = FireballState::Exploding; // trúng enemy → nổ
        m_currentFrame = 0;
        m_animTimer    = 0.f;
    }
}

void Fireball::updateFlyAnimation(float dt)
{
    if (m_state != FireballState::Flying)
        return;

    m_animTimer += dt;
    if (m_animTimer >= FLY_FRAME_TIME)
    {
        m_animTimer = 0.f;
        m_currentFrame = (m_currentFrame + 1) % 4;
    }
    m_sprite.setTextureRect(FireballSprite::FlyFrames[m_currentFrame]);
    m_sprite.setOrigin({
        FireballSprite::FlyFrames[m_currentFrame].size.x / 2.f,
        FireballSprite::FlyFrames[m_currentFrame].size.y / 2.f
    });
}

void Fireball::updateExplodeAnimation(float dt)
{
    if (m_state != FireballState::Exploding)
        return;

    m_animTimer += dt;
    m_currentFrame = static_cast<int>(m_animTimer / EXPLODE_FRAME_TIME);
    if (m_currentFrame >= 3)
    {
        m_state = FireballState::Dead;
        return;
    }
    m_sprite.setTextureRect(FireballSprite::ExplodeFrames[m_currentFrame]);
    m_sprite.setOrigin({
        FireballSprite::ExplodeFrames[m_currentFrame].size.x / 2.f,
        FireballSprite::ExplodeFrames[m_currentFrame].size.y / 2.f
    });
}

bool Fireball::collidesWithMap(const IMapManager& map, float x, float y) const
{
    return map.isSolid(x, y);
}

sf::FloatRect Fireball::getHitbox() const
{
    return m_sprite.getGlobalBounds();
}

void Fireball::render(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}