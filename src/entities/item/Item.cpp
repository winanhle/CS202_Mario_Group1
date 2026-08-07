#include "Item.h"
#include "../../interfaces/IPlayerManager.h"

Item::Item(float x, float y, 
           sf::Texture& texture):
           m_position(x, y),
           m_size((float)texture.getSize().x, (float)texture.getSize().y),
           m_sprite(texture),
           m_collected(false) {
    m_sprite.setPosition(m_position);
    m_sprite.setScale({0.5f, 0.5f});
};

void Item::update(float deltaTime) {
    
}

void Item::render(sf::RenderWindow& window) const {
    if (!m_collected) {
        window.draw(m_sprite);
    }
}

sf::FloatRect Item::getHitbox() const
{
    return m_sprite.getGlobalBounds();
}

bool Item::checkCollision(IPlayerManager* player)
{
    if (m_collected || player == nullptr)
        return false;

    if (player->getHitbox().findIntersection(getHitbox()))
    {
        m_collected = true;
        OnInteract(player);
        return true;
    }

    return false;
}

bool Item::isCollected() const
{
    return m_collected;
}

void Item::setCollected(bool collected)
{
    m_collected = collected;
}

sf::Vector2f Item::getPosition() const
{
    return m_position;
}

void Item::setPosition(const sf::Vector2f& pos)
{
    m_position = pos;
    m_sprite.setPosition(m_position);
}