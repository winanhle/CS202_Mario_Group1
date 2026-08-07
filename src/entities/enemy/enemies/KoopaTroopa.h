#pragma once

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

class KoopaTroopa : public Enemy
{
public:
    KoopaTroopa(float x, float y, sf::Texture& texture)
        : Enemy(x, y, texture) {}

    void onPlayerCollision(IPlayerManager* player) override
    {
        if (!player)
            return;

        player->takeDamage();
    }
};