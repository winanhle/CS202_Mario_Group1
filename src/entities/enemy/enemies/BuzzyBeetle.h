#pragma once

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

class BuzzyBeetle : public Enemy
{
public:
    BuzzyBeetle(float x, float y, sf::Texture& texture)
        : Enemy(x, y, texture) {}

    void onPlayerCollision(IPlayerManager* player) override
    {
        if (!player)
            return;

        player->takeDamage();
    }
};