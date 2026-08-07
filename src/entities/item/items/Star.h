#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class Star : public Item
{
public:
    Star(float x, float y, sf::Texture& texture)
        : Item(x, y, texture) {}

    void OnInteract(IPlayerManager* player) override
    {
        player->collectCoin(1);
    }
};