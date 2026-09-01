#include "FireBar.h"
#include <algorithm>
#include <cmath>

FireBar::FireBar(float cx, float cy,
                 const sf::Texture& texture,
                 int fireCount,
                 float speed,
                 bool clockwise,
                 float initialAngle)
    : m_center(cx, cy),
      m_fireCount(std::max(1, fireCount)),
      m_speed(speed),
      m_clockwise(clockwise),
      m_angle(initialAngle),
      m_sprite(texture)
{
    constexpr float DEG_TO_RAD = 3.14159265359f / 180.f;
    constexpr float TWO_PI = 6.28318530718f;

    // If speed is provided in degrees/sec (> 2*PI, e.g. 60, 90, 180), convert to rad/s
    if (std::abs(m_speed) > TWO_PI)
    {
        m_speed *= DEG_TO_RAD;
    }

    // If initial angle is provided in degrees (> 2*PI), convert to radians
    if (std::abs(m_angle) > TWO_PI)
    {
        m_angle *= DEG_TO_RAD;
    }

    m_sprite.setTextureRect(FireballSprite::FlyFrames[0]);
    m_sprite.setOrigin({
        FireballSprite::FlyFrames[0].size.x / 2.f,
        FireballSprite::FlyFrames[0].size.y / 2.f
    });
}

void FireBar::update(float dt)
{
    // Advance rotation angle
    float dir = m_clockwise ? 1.0f : -1.0f;
    m_angle += dir * m_speed * dt;

    // Keep angle normalized in [0, 2*PI)
    constexpr float TWO_PI = 6.28318530718f;
    while (m_angle >= TWO_PI)
        m_angle -= TWO_PI;
    while (m_angle < 0.f)
        m_angle += TWO_PI;

    // Advance 4-frame fireball animation
    m_animTimer += dt;
    if (m_animTimer >= FLY_FRAME_TIME)
    {
        m_animTimer = 0.f;
        m_currentFrame = (m_currentFrame + 1) % 4;
    }
}

void FireBar::render(sf::RenderWindow& window) const
{
    m_sprite.setTextureRect(FireballSprite::FlyFrames[m_currentFrame]);
    m_sprite.setOrigin({
        FireballSprite::FlyFrames[m_currentFrame].size.x / 2.f,
        FireballSprite::FlyFrames[m_currentFrame].size.y / 2.f
    });

    float cosA = std::cos(m_angle);
    float sinA = std::sin(m_angle);

    for (int i = 0; i < m_fireCount; ++i)
    {
        float r = BASE_OFFSET + static_cast<float>(i) * SPACING;
        float x = m_center.x + r * cosA;
        float y = m_center.y + r * sinA;

        m_sprite.setPosition({x, y});
        window.draw(m_sprite);
    }
}

bool FireBar::checkCollision(const sf::FloatRect& playerBox) const
{
    float cosA = std::cos(m_angle);
    float sinA = std::sin(m_angle);

    for (int i = 0; i < m_fireCount; ++i)
    {
        float r = BASE_OFFSET + static_cast<float>(i) * SPACING;
        float x = m_center.x + r * cosA;
        float y = m_center.y + r * sinA;

        sf::FloatRect fireBox(
            { x - HITBOX_RADIUS, y - HITBOX_RADIUS },
            { HITBOX_RADIUS * 2.f, HITBOX_RADIUS * 2.f }
        );

        if (playerBox.findIntersection(fireBox).has_value())
        {
            return true;
        }
    }

    return false;
}
