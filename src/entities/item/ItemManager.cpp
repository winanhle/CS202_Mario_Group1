#include "ItemManager.h"

#include "../../interfaces/IPlayerManager.h"

#include "items/Coin.h"
#include "items/Mushroom.h"
#include "items/FireFlower.h"
#include "items/Star.h"

#include <algorithm>
#include <stdexcept>

ItemManager::ItemManager() = default;

void ItemManager::initialize()
{
    m_items.clear();

    if (!m_coinTextures[0].loadFromFile(                        //Load Coin
            "assets/texture/item/Coin.PNG"))
        throw std::runtime_error("Failed to load Coin.PNG");

    if (!m_coinTextures[1].loadFromFile(
            "assets/texture/item/Coin1.PNG"))
        throw std::runtime_error("Failed to load Coin1.PNG");

    if (!m_coinTextures[2].loadFromFile(
            "assets/texture/item/Coin2.PNG"))
        throw std::runtime_error("Failed to load Coin2.PNG");

    if (!m_coinTextures[3].loadFromFile(
            "assets/texture/item/Coin3.PNG"))
        throw std::runtime_error("Failed to load Coin3.PNG");

    if (!m_fireFlowerTextures[0].loadFromFile(                   //Load Flower
            "assets/texture/item/FireFlower.PNG"))
        throw std::runtime_error("Failed to load FireFlower.PNG");

    if (!m_fireFlowerTextures[1].loadFromFile(
            "assets/texture/item/FireFlower1.PNG"))
        throw std::runtime_error("Failed to load FireFlower1.PNG");

    if (!m_fireFlowerTextures[2].loadFromFile(
            "assets/texture/item/FireFlower2.PNG"))
        throw std::runtime_error("Failed to load FireFlower2.PNG");

    if (!m_fireFlowerTextures[3].loadFromFile(
            "assets/texture/item/FireFlower3.PNG"))
        throw std::runtime_error("Failed to load FireFlower3.PNG");

    if (!m_mushroomTexture.loadFromFile(
            "assets/texture/item/Mushroom.PNG"))
        throw std::runtime_error("Failed to load Mushroom.PNG");

    if (!m_starTexture.loadFromFile(
            "assets/texture/item/Star.PNG"))
        throw std::runtime_error("Failed to load Star.PNG");

    std::array<sf::Texture*, 4> coinFrames{
        &m_coinTextures[0],
        &m_coinTextures[1],
        &m_coinTextures[2],
        &m_coinTextures[3]
    };

    std::array<sf::Texture*, 4> flowerFrames{
        &m_fireFlowerTextures[0],
        &m_fireFlowerTextures[1],
        &m_fireFlowerTextures[2],
        &m_fireFlowerTextures[3]
    };

    m_items.push_back(
        std::make_unique<Coin>(
            120.f,
            160.f,
            coinFrames
        )
    );

    m_items.push_back(
        std::make_unique<Mushroom>(
            240.f,
            160.f,
            m_mushroomTexture
        )
    );

    m_items.push_back(
        std::make_unique<FireFlower>(
            320.f,
            160.f,
            flowerFrames
        )
    );

    m_items.push_back(
        std::make_unique<Star>(
            400.f,
            160.f,
            m_starTexture
        )
    );
}

void ItemManager::update(float deltaTime)
{
    if (!m_player)
        return;

    for (auto& item : m_items)
    {
        if (item->isCollected())
            continue;

        // Update animation / movement
        item->update(deltaTime);

        // Check player-item collision
        item->checkCollision(m_player);
        if (m_player2)
            item->checkCollision(m_player2);
    }

    // Remove collected items
    m_items.erase(
        std::remove_if(
            m_items.begin(),
            m_items.end(),
            [](const std::unique_ptr<Item>& item)
            {
                return item->isCollected();
            }
        ),
        m_items.end()
    );
}

void ItemManager::render(sf::RenderWindow& window) const
{
    for (const auto& item : m_items)
    {
        if (!item->isCollected())
        {
            item->render(window);
        }
    }
}

int ItemManager::getItemCount() const
{
    return static_cast<int>(m_items.size());
}

// =============================================================================
//  SPAWN API  – called by MapManager on tile interactions
// =============================================================================

void ItemManager::spawnCoinPop(float worldX, float worldY)
{
    // Award the coin immediately to the player (visual pop is handled by MapManager)
    if (m_player)
        m_player->collectCoin(1);
}

void ItemManager::spawnMushroom(float worldX, float worldY)
{
    m_items.push_back(
        std::make_unique<Mushroom>(worldX, worldY, m_mushroomTexture)
    );
}

void ItemManager::spawnFireFlower(float worldX, float worldY)
{
    std::array<sf::Texture*, 4> flowerFrames{
        &m_fireFlowerTextures[0],
        &m_fireFlowerTextures[1],
        &m_fireFlowerTextures[2],
        &m_fireFlowerTextures[3]
    };
    m_items.push_back(
        std::make_unique<FireFlower>(worldX, worldY, flowerFrames)
    );
}