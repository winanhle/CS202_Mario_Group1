#pragma once
#include <SFML/Graphics.hpp>

class IPlayerManager;
class Enemy {
protected:
    sf::Sprite m_sprite;

    bool m_alive = true;
    bool m_dead = false;

public:
    Enemy(float x,
          float y,
          sf::Texture& texture);
    virtual ~Enemy() = default;

    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) const;

    virtual void onPlayerCollision(IPlayerManager* player) = 0;
    virtual void onStomp();

    sf::FloatRect getHitbox() const;

    bool isDead() const
    {
        return m_dead;
    }
};