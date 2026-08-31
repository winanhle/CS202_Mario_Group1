#pragma once

#include <SFML/Graphics.hpp>
#include <array>

#include "../entities/map/MapManager.h"

/**
 * @class MenuBackground
 * @brief Helper class to render an animated scrolling backdrop on the main menu.
 * Uses MapManager to load and draw a level map, plus a small walking Mario.
 */
class MenuBackground
{
public:
    MenuBackground() = default;
    ~MenuBackground() = default;

    bool load();
    void update(float dt);
    void render(sf::RenderWindow& window) const;

private:
    // Scrolling map
    MapManager m_mapManager;
    float      m_bgScrollX = 0.f;
    static constexpr float BG_SPEED = 20.f; // map-px/s — slow pan (1120px map takes ~40s)

    // Mario walk animation & physics
    sf::Texture  m_marioTexture;
    sf::Sprite   m_marioSprite{m_marioTexture};
    float        m_marioX     = 84.f;   // start at player spawn x
    float        m_marioY     = 192.f;  // above ground (row 14 top = 208, minus sprite height)
    float        m_marioVy    = 0.f;
    bool         m_isJumping  = false;
    
    float        m_animTimer  = 0.f;
    int          m_animFrame  = 0;
    
    float        m_viewWidth  = 341.f;  // 800 / (600/256) ≈ 341
    int          m_tileSize   = 16;

    // Map: 1120x256. Mario is 16x18px in map space.
    static constexpr float WALK_SPEED    = 75.f;   // map-px/s (covers pits easily)
    static constexpr float FRAME_TIME    = 0.11f;  // seconds per walk frame
    static constexpr float GRAVITY       = 600.f;  // map-px/s²
    static constexpr float JUMP_VELOCITY = -310.f; // map-px/s (generates 77px long jump arc)
    
    static constexpr float MARIO_W = 16.f;
    static constexpr float MARIO_H = 18.f;
    
    // Flagpole fade
    static constexpr float FLAGPOLE_X    = 1024.f; // column 64 * 16 = 1024 (flagpole area)
    static constexpr float FADE_DURATION = 1.2f;   // seconds for full fade cycle
    float m_fadeTimer = 0.f;
    bool  m_fading    = false;
    bool  m_fadeIn    = false; // false = fading out, true = fading back in
    
    // Frame rects from Mario.cpp: walk1={1,6,16,18}, walk2={18,6,16,18}, jump={52,6,16,18}
    const std::array<sf::IntRect, 3> MARIO_FRAMES = {{
        sf::IntRect({1,  6}, {16, 18}),
        sf::IntRect({18, 6}, {16, 18}),
        sf::IntRect({52, 6}, {16, 18})  // Jump frame
    }};
};
