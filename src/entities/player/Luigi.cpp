#include "Luigi.h"
#include "forms/NormalForm.h"

Luigi::Luigi() {
    m_currentForm = std::make_unique<NormalForm>();
}

void Luigi::setupStats() {
    m_maxSpeed = 190.0f;
    m_acceleration = 750.0f;
    m_friction = 0.85f;
    m_jumpVelocity = -480.0f;
    m_gravity = 1100.0f;

    m_maxHealth = 80;
    m_attackPower = 90;
    m_defense = 5;
}
