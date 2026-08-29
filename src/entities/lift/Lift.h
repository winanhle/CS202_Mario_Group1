#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

/**
 * @class Lift
 * @brief A moving platform that oscillates automatically within a range.
 *
 * Two motion types:
 *   UpDown    – oscillates vertically
 *   LeftRight – oscillates horizontally
 *
 * Two sizes (measured in 16px tile "holes"):
 *   4 holes → 64 px wide
 *   6 holes → 96 px wide
 *
 * Movement uses a sine wave so the platform accelerates and decelerates
 * smoothly at each end, with no abrupt direction snapping.
 *
 * getDelta() returns the displacement since the last frame so that the
 * LiftManager can carry any rider by exactly that amount.
 */
class Lift
{
public:
    enum class MotionType { UpDown, LeftRight };

    /**
     * @param x         World X of the spawn point (left edge of platform)
     * @param y         World Y of the spawn point (top edge of platform)
     * @param motion    UpDown or LeftRight
     * @param holes     4 or 6 (platform width in tile units)
     * @param texture   Lift texture (single horizontal strip)
     * @param range     Half-amplitude of oscillation in pixels
     * @param speed     Linear speed in px/s (converted to angular velocity)
     * @param tileSize  Map tile size in pixels (default 16)
     */
    Lift(float x, float y,
         MotionType motion, int holes,
         sf::Texture& texture,
         float range, float speed,
         int tileSize = 16);

    void update(float dt);
    void render(sf::RenderWindow& window) const;

    /** Axis-aligned bounding box of the platform (world space). */
    sf::FloatRect getHitbox() const;

    /**
     * @brief Displacement applied this frame.
     * LiftManager adds this vector to any rider's position.
     */
    sf::Vector2f getDelta() const { return m_delta; }

    /** Width of the platform sprite in pixels. */
    float getWidth()  const { return m_width; }
    /** Height of the platform sprite in pixels (always 8 px = tileSize/2). */
    float getHeight() const { return m_height; }

private:
    sf::Sprite   m_sprite;
    MotionType   m_motionType;

    sf::Vector2f m_origin;       // spawn position = oscillation center
    sf::Vector2f m_position;     // current top-left corner
    sf::Vector2f m_prevPosition; // position last frame
    sf::Vector2f m_delta;        // m_position - m_prevPosition this frame

    float m_range;               // half-amplitude (pixels)
    float m_angularSpeed;        // radians per second  =  speed / range
    float m_phase = 0.f;         // current phase [0, 2π)

    float m_width;               // computed from holes * tileSize
    float m_height;              // tileSize / 2
};
