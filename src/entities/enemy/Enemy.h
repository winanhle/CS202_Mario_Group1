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
    float m_gravity = 800.f;
    float m_maxFallSpeed = 400.f;
    
    int m_direction = -1; // -1 = left, 1 = right

    bool m_playerOverlapping[2] = { false, false }; // see wasPlayerOverlapping()/setPlayerOverlapping()

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
    
    virtual bool usesPhysics() const {
        return true;
    }

    void applyGravity(float dt);
    void move(float dt);
    void reverseDirection();

    void setSpriteTexture(sf::Texture& texture)
    {
        sf::FloatRect oldBounds = m_sprite.getGlobalBounds();
        m_sprite.setTexture(texture, true);
        sf::FloatRect newBounds = m_sprite.getGlobalBounds();

        float widthDiff = newBounds.size.x - oldBounds.size.x;
        float heightDiff = newBounds.size.y - oldBounds.size.y;
        m_sprite.move({-widthDiff / 2.f, -heightDiff});
    }

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

    float getMoveSpeed() const
    {
        return m_moveSpeed;
    }

    int getDirection() const
    {
        return m_direction;
    }

    bool isDead() const
    {
        return m_dead;
    }

    bool wasPlayerOverlapping(int playerIndex) const
    {
        return m_playerOverlapping[playerIndex];
    }

    void setPlayerOverlapping(int playerIndex, bool overlapping)
    {
        m_playerOverlapping[playerIndex] = overlapping;
    }

    void setPositionY(float y) {
        auto position = m_sprite.getPosition();
        m_sprite.setPosition({position.x, y});
    }
};