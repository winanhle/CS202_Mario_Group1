#pragma once

#include "../Item.h"
#include "../ItemSprite.h"
#include "../../../interfaces/IPlayerManager.h"

class Mushroom : public Item
{
public:
    Mushroom(float x, float y, sf::Texture& texture)
        : Item(x, y, texture) {}

    void OnInteract(IPlayerManager* player) override
    {
       player->collectCoin(1);
    }
};