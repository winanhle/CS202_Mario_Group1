#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "FormOffsets.h"

enum class PowerUpType { Mushroom, FireFlower, Star };

/**
 * @interface IPlayerForm
 * @brief Interface cho các dạng biến hình của player (Normal, Super, Fire).
 *
 * Mỗi form được khởi tạo với CharacterFormOffsets của nhân vật sở hữu nó,
 * đảm bảo frame sprite và hitbox luôn phù hợp với sprite sheet của nhân vật đó.
 */
class IPlayerForm {
public:
    virtual ~IPlayerForm() = default;
    virtual sf::IntRect  getWalkFrame1()  const = 0;
    virtual sf::IntRect  getWalkFrame2()  const = 0;
    virtual sf::IntRect  getJumpFrame()   const = 0;
    virtual sf::IntRect  getDeathFrame()  const = 0;
    virtual sf::Vector2f getHitboxSize()  const = 0;
    virtual std::unique_ptr<IPlayerForm> takeDamage() const = 0;
    virtual std::unique_ptr<IPlayerForm> evolve(PowerUpType type) const = 0;

    /**
     * @brief Tick các timer nội bộ của form mỗi frame.
     * Mặc định không làm gì; FireForm override để tick shoot cooldown.
     */
    virtual void update(float deltaTime) { (void)deltaTime; }
};
