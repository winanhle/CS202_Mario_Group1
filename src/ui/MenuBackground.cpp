#include "MenuBackground.h"
#include <iostream>

bool MenuBackground::load()
{
    bool success = true;

    // Load actual level map for the backdrop
    m_mapManager.initialize();
    m_mapManager.loadMap("assets/map/stage1.tmx");
    m_tileSize = m_mapManager.getTileSize();

    if (!m_marioTexture.loadFromFile("assets/texture/hero/mario.png"))
    {
        std::cerr << "Failed to load mario.png for MenuBackground\n";
        success = false;
    }
    else
    {
        m_marioSprite.setTexture(m_marioTexture);
        m_marioSprite.setTextureRect(MARIO_FRAMES[0]);
        m_marioSprite.setScale({1.0f, 1.0f});
    }

    // Compute view dimensions once
    float mapH = static_cast<float>(m_mapManager.getMapPixelSize().y);
    if (mapH > 0.f)
    {
        float scale = 600.f / mapH;
        m_viewWidth = 800.f / scale;
    }

    return success;
}

void MenuBackground::update(float dt)
{
    // Handle fade transition
    if (m_fading)
    {
        m_fadeTimer += dt;
        float halfDuration = FADE_DURATION / 2.f;
        
        if (!m_fadeIn)
        {
            if (m_fadeTimer >= halfDuration)
            {
                m_fadeIn = true;
                m_fadeTimer = 0.f;
                m_marioX  = 84.f;
                m_marioY  = 192.f;
                m_marioVy = 0.f;
                m_isJumping = false;
                m_bgScrollX = 0.f;
            }
        }
        else
        {
            if (m_fadeTimer >= halfDuration)
            {
                m_fading = false;
                m_fadeIn = false;
                m_fadeTimer = 0.f;
            }
        }
        
        if (!m_fadeIn)
        {
            m_mapManager.update(dt);
            return;
        }
    }

    m_mapManager.update(dt);

    float mapW = static_cast<float>(m_mapManager.getMapPixelSize().x);
    float mapH = static_cast<float>(m_mapManager.getMapPixelSize().y);
    float ts   = static_cast<float>(m_tileSize);

    // 1. Move Mario forward horizontally
    m_marioX += WALK_SPEED * dt;

    // 2. Horizontal Collision (Right side)
    // Check points at top, middle, and bottom of Mario's right edge
    bool hitWallRight = m_mapManager.isSolid(m_marioX + MARIO_W, m_marioY + 2.f) ||
                        m_mapManager.isSolid(m_marioX + MARIO_W, m_marioY + MARIO_H / 2.f) ||
                        m_mapManager.isSolid(m_marioX + MARIO_W, m_marioY + MARIO_H - 2.f);
    
    if (hitWallRight)
    {
        // Snap to tile boundary on the left of the wall
        int tileX = static_cast<int>(m_marioX + MARIO_W) / m_tileSize;
        m_marioX = static_cast<float>(tileX) * ts - MARIO_W - 0.1f; // push slightly back
    }

    // 3. Apply gravity
    m_marioVy += GRAVITY * dt;
    m_marioY += m_marioVy * dt;

    // 4. Vertical Collision (Floor and Ceiling)
    float feetY = m_marioY + MARIO_H;
    bool onGround = false;

    if (m_marioVy >= 0.f)
    {
        // Check under feet (left and right)
        bool hitFloorLeft  = m_mapManager.isSolid(m_marioX + 2.f, feetY);
        bool hitFloorRight = m_mapManager.isSolid(m_marioX + MARIO_W - 2.f, feetY);
        
        if (hitFloorLeft || hitFloorRight)
        {
            m_marioVy = 0.f;
            m_isJumping = false;
            // Snap to floor
            int tileY = static_cast<int>(feetY) / m_tileSize;
            m_marioY = static_cast<float>(tileY) * ts - MARIO_H;
            onGround = true;
        }
        else
        {
            m_isJumping = true;
        }
    }
    else // Moving up
    {
        // Check head (left and right)
        bool hitCeilLeft  = m_mapManager.isSolid(m_marioX + 2.f, m_marioY);
        bool hitCeilRight = m_mapManager.isSolid(m_marioX + MARIO_W - 2.f, m_marioY);
        
        if (hitCeilLeft || hitCeilRight)
        {
            m_marioVy = 0.f;
            // Snap to ceiling
            int tileY = static_cast<int>(m_marioY) / m_tileSize;
            m_marioY = static_cast<float>(tileY + 1) * ts;
        }
    }

    // 5. AI Jump Logic (Only jump when firmly on the ground)
    if (onGround)
    {
        // Jump if we are blocked horizontally (fallback)
        bool shouldJump = hitWallRight;

        // Anticipate obstacles to jump over them smoothly without stopping
        if (!shouldJump)
        {
            // For walls/pipes: scan ahead by 12px so he launches up before hitting them
            float lookAheadWallX = m_marioX + MARIO_W + 12.f;
            bool wallAhead = m_mapManager.isSolid(lookAheadWallX, m_marioY + MARIO_H - 8.f);
            
            // For pits: scan right at the edge of his front foot (3px ahead) so he jumps off the cliff edge, not 14px early!
            float lookAheadPitX = m_marioX + MARIO_W + 3.f;
            bool pitAhead = !m_mapManager.isSolid(lookAheadPitX, m_marioY + MARIO_H + 2.f);

            if (wallAhead || pitAhead)
            {
                shouldJump = true;
            }
        }

        if (shouldJump)
        {
            m_marioVy = JUMP_VELOCITY;
            m_isJumping = true;
            onGround = false;
        }
    }

    // 6. Flagpole detection
    if (!m_fading && m_marioX >= FLAGPOLE_X)
    {
        m_fading = true;
        m_fadeIn = false;
        m_fadeTimer = 0.f;
    }

    // 7. Hard-lock camera to Mario to prevent stutter
    float cameraTarget = m_marioX - m_viewWidth * 0.3f;
    float maxScroll = mapW - m_viewWidth;
    
    // Clamp camera
    if (cameraTarget < 0.f) cameraTarget = 0.f;
    if (maxScroll > 0.f && cameraTarget > maxScroll) cameraTarget = maxScroll;
    
    m_bgScrollX = cameraTarget;

    // 8. Fallback if he somehow falls below the map
    if (m_marioY > mapH + 32.f)
    {
        m_fading = true;
        m_fadeIn = false;
        m_fadeTimer = FADE_DURATION / 2.f; // force immediate reset and fade in
    }

    // 9. Update sprite and animation
    m_marioSprite.setPosition({m_marioX, m_marioY});

    if (m_isJumping)
    {
        m_marioSprite.setTextureRect(MARIO_FRAMES[2]);
    }
    else
    {
        // If we are blocked by a wall and not jumping (e.g., waiting to jump), keep walking animation
        m_animTimer += dt;
        if (m_animTimer >= FRAME_TIME)
        {
            m_animTimer = 0.f;
            m_animFrame = (m_animFrame + 1) % 2;
            m_marioSprite.setTextureRect(MARIO_FRAMES[m_animFrame]);
        }
    }
}

void MenuBackground::render(sf::RenderWindow& window) const
{
    sf::View originalView = window.getView();

    // Map view: sized to show m_viewWidth map pixels across 800 screen pixels
    float mapH = static_cast<float>(m_mapManager.getMapPixelSize().y);
    float viewH = (mapH > 0.f) ? mapH : 256.f;

    sf::View mapView({0.f, 0.f}, {m_viewWidth, viewH});
    mapView.setCenter({m_bgScrollX + m_viewWidth / 2.f, viewH / 2.f});
    window.setView(mapView);

    // Draw map + Mario (both in map-pixel space)
    m_mapManager.render(window);
    window.draw(m_marioSprite);

    // Restore screen-space view for overlays
    window.setView(originalView);

    // Compute overlay alpha — base dimming (100) + fade effect
    float fadeAlpha = 100.f; // base dim for text readability
    
    if (m_fading)
    {
        float halfDuration = FADE_DURATION / 2.f;
        float progress = m_fadeTimer / halfDuration;
        if (progress > 1.f) progress = 1.f;
        
        if (!m_fadeIn)
        {
            // Fading out: 100 → 255
            fadeAlpha = 100.f + progress * 155.f;
        }
        else
        {
            // Fading in: 255 → 100
            fadeAlpha = 255.f - progress * 155.f;
        }
    }

    sf::RectangleShape overlay({800.f, 600.f});
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
    window.draw(overlay);
}
