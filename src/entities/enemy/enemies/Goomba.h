#pragma once

#include "../EnemySprite.h"
#include "../Enemy.h"
#include "../../../interfaces/IPlayerManager.h"

class Goomba : public Enemy
{
public:
    Goomba(float x, float y, sf::Texture& texture)
        : Enemy(x, y, texture)
    {
    }

    void update(float deltaTime) override
    {
        // Do nothing for now.
        // First test: just display Goomba.
    }

    void onPlayerCollision(IPlayerManager* player) override
    {
        if (!player)
            return;

        player->takeDamage();
    }
};