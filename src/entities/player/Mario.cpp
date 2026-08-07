#include "Mario.h"
#include "forms/NormalForm.h"

Mario::Mario() {
    m_currentForm = std::make_unique<NormalForm>();
}

void Mario::setupStats() {
    m_maxSpeed = 200.0f;
    m_acceleration = 800.0f;
    m_friction = 0.8f;        
    m_jumpVelocity = -450.0f; 
    m_gravity = 1200.0f;

    m_maxHealth = 100;
    m_attackPower = 80;
    m_defense = 10;
}
