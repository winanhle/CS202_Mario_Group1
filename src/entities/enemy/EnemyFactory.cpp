#include "EnemyFactory.h"
#include "EnemySprite.h"
#include "enemies/Goomba.h"
#include "enemies/BuzzyBeetle.h"
#include "enemies/KoopaTroopa.h"
#include "enemies/Boss.h"
#include "enemies/BossFireBall.h"
#include <iostream>

void EnemyFactory::preloadTextures() {
    // Preload all enemy textures to ensure assets exist and avoid runtime stutter
    Goomba::walk1();
    Goomba::walk2();
    Goomba::dead();

    BuzzyBeetle::left1();
    BuzzyBeetle::left2();
    BuzzyBeetle::shell();
    BuzzyBeetle::right1();
    BuzzyBeetle::right2();

    KoopaTroopa::left1();
    KoopaTroopa::left2();
    KoopaTroopa::right1();
    KoopaTroopa::right2();
    KoopaTroopa::shell1();
    KoopaTroopa::shell2();

    Boss::walkLeft1();
    Boss::walkLeft2();
    Boss::walkRight1();
    Boss::walkRight2();
    Boss::attackLeft1();
    Boss::attackLeft2();
    Boss::attackRight1();
    Boss::attackRight2();
    Boss::shell1();
    Boss::shell2();

    BossFireball::left1();
    BossFireball::left2();
    BossFireball::right1();
    BossFireball::right2();
}

std::unique_ptr<Enemy> EnemyFactory::createEnemy(
    const EntitySpawnData& spawnData,
    FireballSpawnCallback onSpawnFireball
) {
    std::unique_ptr<Enemy> enemy = nullptr;

    if (spawnData.type == "Goomba") {
        enemy = std::make_unique<Goomba>(spawnData.x, spawnData.y);
    }
    else if (spawnData.type == "KoopaTroopa") {
        enemy = std::make_unique<KoopaTroopa>(spawnData.x, spawnData.y);
    }
    else if (spawnData.type == "BuzzyBeetle") {
        enemy = std::make_unique<BuzzyBeetle>(spawnData.x, spawnData.y);
    }
    else if (spawnData.type == "Boss") {
        enemy = std::make_unique<Boss>(spawnData.x, spawnData.y, std::move(onSpawnFireball));
    }
    else {
        std::cerr << "[EnemyFactory] Unknown enemy type: " << spawnData.type << std::endl;
        return nullptr;
    }

    if (enemy) {
        if (spawnData.moveSpeed > 0.f) {
            enemy->setMoveSpeed(spawnData.moveSpeed);
        }
        if (spawnData.direction == "right" || spawnData.direction == "1") {
            if (enemy->getDirection() < 0) {
                enemy->reverseDirection();
            }
        }
    }

    return enemy;
}
