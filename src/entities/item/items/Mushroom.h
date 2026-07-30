#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class Mushroom : public Item
{
public:
    Mushroom(float x, float y, sf::Texture& texture)
        : Item(x, y, texture, ItemSprite::GreenMushroom) {}

    void OnInteract(IPlayerManager* player) override
    {
       player->collectPowerUp(0);
    }

    void render(sf::RenderWindow& window) const override
    {
        window.draw(m_sprite);
    }
};
