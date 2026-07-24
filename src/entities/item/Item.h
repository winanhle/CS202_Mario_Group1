#pragma once

#include <SFML/Graphics.hpp>

class IPlayerManager;

class Item
{
protected:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    bool m_collected;

public:
    Item(float x, float y, float width = 16.f, float height = 16.f);
    virtual ~Item() = default;

    // Lifecycle
    virtual void update(float deltaTime);
    virtual void render(sf::RenderWindow& window) const = 0;

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
};