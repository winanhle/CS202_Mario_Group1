#include "Lift.h"

Lift::Lift(float x, float y,
           MotionType motion, int holes,
           sf::Texture& texture,
           float range, float speed,
           int tileSize)
    : m_sprite(texture)
    , m_motionType(motion)
    , m_range(range)
{
    sf::Vector2u texSize = texture.getSize();

    // ── Texture crop ──────────────────────────────────────────────────────────
    // holes=4 → use the left 2/3 of the sprite sheet
    // holes=6 → use the full sprite sheet
    int cropWidth = (holes == 6)
        ? static_cast<int>(texSize.x)
        : static_cast<int>(texSize.x * 2 / 3);

    m_sprite.setTextureRect(sf::IntRect({0, 0}, {cropWidth, static_cast<int>(texSize.y)}));

    // ── Display dimensions ────────────────────────────────────────────────────
    // The platform is rendered at native width (1:1 pixel mapping on X) and
    // scaled only on Y to fit tileSize/2 height.
    m_width  = static_cast<float>(cropWidth);
    m_height = static_cast<float>(tileSize / 2);

    float scaleY = m_height / static_cast<float>(texSize.y);
    m_sprite.setScale({1.f, scaleY});

    // ── Oscillation setup ─────────────────────────────────────────────────────
    m_origin   = {x, y};
    m_position = m_origin;
    m_sprite.setPosition(m_position);
    m_prevPosition = m_position;

    // Angular velocity: ω = linearSpeed / range  (so peak linear speed == speed)
    // Guard against zero range to avoid NaN
    m_angularSpeed = (m_range > 0.f) ? (speed / m_range) : 0.f;
}

void Lift::update(float dt)
{
    m_prevPosition = m_position;

    m_phase += m_angularSpeed * dt;
    if (m_phase > 2.f * static_cast<float>(M_PI))
        m_phase -= 2.f * static_cast<float>(M_PI);

    float offset = m_range * std::sin(m_phase);

    if (m_motionType == MotionType::UpDown)
        m_position = {m_origin.x, m_origin.y + offset};
    else
        m_position = {m_origin.x + offset, m_origin.y};

    m_sprite.setPosition(m_position);
    m_delta = m_position - m_prevPosition;
}

void Lift::render(sf::RenderWindow& window) const
{
    window.draw(m_sprite);
}

sf::FloatRect Lift::getHitbox() const
{
    return m_sprite.getGlobalBounds();
}
