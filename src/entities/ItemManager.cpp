#include "ItemManager.h"
#include <SFML/Graphics.hpp>

ItemManager::ItemManager()
    : m_itemCount(0)
{
}

void ItemManager::initialize()
{
    // TODO: Dinh Anh - Initialize item manager
    // - Load item sprites
    // - Spawn initial items
    // - Set up item properties
}

void ItemManager::update(float deltaTime)
{
    // TODO: Dinh Anh - Update all items
    // - Apply physics (gravity for falling items)
    // - Handle animations
    // - Check for collected items
    // - Spawn new items
}

void ItemManager::render(sf::RenderWindow& window) const
{
    // TODO: Dinh Anh - Render all items
    // For now, draw placeholder rectangles
    for (int i = 0; i < m_itemCount; ++i)
    {
        sf::RectangleShape item({ 16.0f, 16.0f });
        item.setFillColor(sf::Color::Magenta);
        item.setPosition({ 200.0f + i * 40.0f, 200.0f });
        window.draw(item);
    }
}

int ItemManager::getItemCount() const
{
    return m_itemCount;
}
