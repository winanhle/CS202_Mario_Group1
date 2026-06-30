#include "Mario.h"

void Mario::setupStats() {
    // Platforming Mechanics: Đầm, dễ kiểm soát
    m_maxSpeed = 200.0f;
    m_acceleration = 800.0f;
    m_friction = 0.8f;        
    m_jumpVelocity = -450.0f; 
    m_gravity = 1200.0f;

    // RPG Mechanics: Glass Cannon
    m_maxHealth = 100;
    m_attackPower = 80;
    m_defense = 10;
}