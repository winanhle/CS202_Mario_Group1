#pragma once

#include <SFML/Graphics.hpp>

class IPlayerManager;

class Item
{
protected:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    sf::Sprite m_sprite;
    
    bool m_onGround = false;

    sf::Vector2f m_velocity{0.f, 0.f};
    float m_gravity = 900.f;
    float m_moveSpeed = 70.f;
    float m_maxFallSpeed = 400.f;

    int m_direction = 1;
    bool m_collected;

public:
    Item(float x, float y, sf::Texture& texture);
    virtual ~Item() = default;

    virtual void update(float deltaTime);
    virtual void render(sf::RenderWindow& window) const;

    // Called by ItemManager right after ground collision snaps the item onto
    // a tile (velocity.y just got zeroed and position snapped). Base does
    // nothing; override this to react to landing, e.g. Star bounces back up.
    virtual void onLanded() {}

    // Item interaction
    virtual void OnInteract(IPlayerManager* player) = 0;

    // Collision
    sf::FloatRect getHitbox() const;
    bool checkCollision(IPlayerManager* player);

    // State
    bool isCollected() const;
    void setCollected(bool collected);

    // Position
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);
    
    void applyGravity(float dt);
    virtual void move(float dt);
    void reverseDirection();

    sf::Vector2f getVelocity() const
    {
        return m_velocity;
    }

    void setVelocityY(float y)
    {
        m_velocity.y = y;
    }

    void setMoveSpeed(float speed)
    {
        m_moveSpeed = speed;
    }

    int getDirection() const
    {
        return m_direction;
    }

    void setPositionY(float y) {
        m_position.y = y;
        m_sprite.setPosition(m_position);
    }
};