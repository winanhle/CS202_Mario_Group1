#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "../projectile/FireballSprite.h"

/**
 * @class FireBar
 * @brief Represents an individual rotating FireBar obstacle.
 *
 * Consists of a chain of N fireballs (default 6) that radiate from a center
 * block pivot and rotate continuously.
 *
 * Mathematical kinematics:
 *   theta(t) = theta_0 + (direction * speed * t)
 *   r_i      = BASE_OFFSET + (i * SPACING)
 *   (x_i, y_i) = (cx + r_i * cos(theta), cy + r_i * sin(theta))
 *
 * Visuals:
 *   Uses Mario's fireball shooting sprites (FlyFrames from FireballSprite.h)
 *   cycling through 4 frames over time.
 */
class FireBar
{
public:
    /**
     * @param cx           Center pivot X in world pixels
     * @param cy           Center pivot Y in world pixels
     * @param texture      Mario texture containing fireball sprites
     * @param fireCount    Number of fireballs in the arm (default 6)
     * @param speed        Angular speed in radians per second (default 2.0 rad/s)
     * @param clockwise    Rotation direction (true = clockwise, false = counter-clockwise)
     * @param initialAngle Starting angle in radians (default 0)
     */
    FireBar(float cx, float cy,
            const sf::Texture& texture,
            int fireCount = 6,
            float speed = 2.0f,
            bool clockwise = true,
            float initialAngle = 0.f);

    void update(float dt);
    void render(sf::RenderWindow& window) const;

    /**
     * @brief Checks if any fireball in the bar intersects the given player hitbox.
     * @param playerBox Axis-aligned bounding box of the player
     * @return True if a collision occurs
     */
    bool checkCollision(const sf::FloatRect& playerBox) const;

    sf::Vector2f getCenter() const { return m_center; }
    float getAngle() const { return m_angle; }
    int getFireCount() const { return m_fireCount; }

private:
    sf::Vector2f m_center;
    int          m_fireCount;
    float        m_speed;
    bool         m_clockwise;
    float        m_angle;

    // Animation
    int   m_currentFrame = 0;
    float m_animTimer    = 0.f;

    // Sprite representation
    mutable sf::Sprite m_sprite; // reused per fireball during render and collision

    static constexpr float FLY_FRAME_TIME = 0.08f;
    static constexpr float SPACING        = 8.0f; // Distance between fireballs (px)
    static constexpr float BASE_OFFSET    = 4.0f; // Distance of innermost fireball from center (px)
    static constexpr float HITBOX_RADIUS  = 3.5f; // Effective radius for collision (px)
};
