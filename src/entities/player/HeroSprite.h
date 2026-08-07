#pragma once
#include <SFML/Graphics.hpp>

namespace HeroSprite
{
    constexpr sf::IntRect WalkFrame1 {{1, 6}, {15, 18}};
    constexpr sf::IntRect WalkFrame2 {{18, 6}, {15, 18}};

    constexpr sf::IntRect JumpFrame  {{52, 6}, {15, 18}};

    constexpr sf::IntRect Idle = WalkFrame1;
}
