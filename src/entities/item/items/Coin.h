#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class Coin : public Item
{
public:
    Coin(float x, float y, sf::Texture& texture)
        : Item(x, y, texture, ItemSprite::Coin) {}

    void OnInteract(IPlayerManager* player) override
    {
        player->collectCoin(1);
    }

    void render(sf::RenderWindow& window) const override
    {
        window.draw(m_sprite);
    }
};