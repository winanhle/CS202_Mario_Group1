#include "Item.h"
#include "../../interfaces/IPlayerManager.h"

Item::Item(float x, float y, 
           sf::Texture& texture, const sf::IntRect& rect):
           m_position(x, y),
           m_size((float)rect.size.x, (float)rect.size.y),
           m_sprite(texture),
           m_collected(false) {
    m_sprite.setTextureRect(rect);
    m_sprite.setPosition(m_position);
};

void Item::update(float deltaTime) {
    
}

sf::FloatRect Item::getHitbox() const
{
    return sf::FloatRect(m_position, m_size);
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
}