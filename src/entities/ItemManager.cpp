#include "ItemManager.h"
#include "../interfaces/IPlayerManager.h"
#include <SFML/Graphics.hpp>

ItemManager::ItemManager()
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
    if (!m_player) return;
    sf::FloatRect playerBox = m_player->getHitbox();

    for (auto& item : m_items) {
        if (item.collected) continue;

        if (playerBox.findIntersection(item.getHitbox())) {
            item.collected = true;
            m_player->collectCoin(item.coinValue);
        }
    }
}

void ItemManager::render(sf::RenderWindow& window) const
{
    // TODO: Dinh Anh - Render all items
    // For now, draw placeholder rectangles
    for (const auto& item : m_items) {
        if (item.collected) continue;
        sf::RectangleShape shape({16.f, 16.f});
        shape.setFillColor(sf::Color::Magenta);
        shape.setPosition({item.x, item.y});
        window.draw(shape);
    }
}

int ItemManager::getItemCount() const
{
    return static_cast<int>(m_items.size());
}
