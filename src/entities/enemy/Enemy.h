#pragma once

#include <SFML/Graphics.hpp>

class IPlayerManager;

class Enemy {
protected:
    sf::Sprite m_sprite;

    bool m_alive = true;
    bool m_dead = false;

    sf::Vector2f m_velocity{0.f, 0.f}; // Movement
    float m_moveSpeed = 40.f;

    int m_direction = -1; // -1 = left, 1 = right

public:
    Enemy(float x,
          float y,
          sf::Texture& texture);

    virtual ~Enemy() = default;

    virtual void update(float dt);

    virtual void render(sf::RenderWindow& window) const;

    virtual void onPlayerCollision(IPlayerManager* player) = 0;
    virtual void onStomp();

    sf::FloatRect getHitbox() const;
    
    void move(float dt);
    void reverseDirection();

    void setMoveSpeed(float speed)
    {
        m_moveSpeed = speed;
    }

    int getDirection() const
    {
        return m_direction;
    }

    bool isDead() const
    {
        return m_dead;
    }
};