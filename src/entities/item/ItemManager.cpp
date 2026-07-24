#include "ItemManager.h"

#include "../../interfaces/IPlayerManager.h"

#include "items/Coin.h"
#include "items/Mushroom.h"
#include "items/FireFlower.h"
#include "items/Star.h"

#include <algorithm>

ItemManager::ItemManager() = default;

void ItemManager::initialize()
{
    m_items.clear();

    // Demo items
    m_items.push_back(std::make_unique<Coin>(120.f, 160.f));
    m_items.push_back(std::make_unique<Coin>(160.f, 160.f));

    // Uncomment when implemented
    // m_items.push_back(std::make_unique<Mushroom>(240.f, 160.f));
    // m_items.push_back(std::make_unique<FireFlower>(320.f, 160.f));
    // m_items.push_back(std::make_unique<Star>(400.f, 160.f));
}

void ItemManager::update(float deltaTime)
{
    if (!m_player)
        return;

    for (auto& item : m_items)
    {
        item->update(deltaTime);
        item->checkCollision(m_player);
    }

    m_items.erase(
        std::remove_if(
            m_items.begin(),
            m_items.end(),
            [](const std::unique_ptr<Item>& item)
            {
                return item->isCollected();
            }),
        m_items.end());
}

void ItemManager::render(sf::RenderWindow& window) const
{
    for (const auto& item : m_items)
    {
        item->render(window);
    }
}

int ItemManager::getItemCount() const
{
    return static_cast<int>(m_items.size());
}