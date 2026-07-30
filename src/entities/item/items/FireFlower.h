#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class FireFlower : public Item
{
public:
    FireFlower(float x, float y, sf::Texture& texture)
        : Item(x, y, texture, ItemSprite::FireFlower) {}

    void OnInteract(IPlayerManager* player) override
    {
        player->collectPowerUp(1);
    }

    void render(sf::RenderWindow& window) const override
    {
        window.draw(m_sprite);
    }
};
