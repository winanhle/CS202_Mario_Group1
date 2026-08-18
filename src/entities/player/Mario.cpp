#include "Mario.h"
#include "forms/NormalForm.h"

static const CharacterFormOffsets MARIO_OFFSETS = {
    // normalForm
    { {{1,  6}, {15, 18}}, {{18,  6}, {15, 18}}, {{52,  6}, {15, 18}}, {15.f, 18.f} },
    // superForm
    { {{1, 31}, {15, 26}}, {{18, 31}, {15, 26}}, {{52, 31}, {15, 26}}, {15.f, 26.f} },
    // fireForm
    { {{1, 64}, {15, 26}}, {{18, 64}, {15, 26}}, {{52, 64}, {15, 26}}, {15.f, 26.f} },
};

Mario::Mario() {
    m_currentForm = std::make_unique<NormalForm>(MARIO_OFFSETS);
}

void Mario::setupStats() {
    m_maxSpeed        = 200.0f;   // WALK
    m_runMaxSpeed     = 330.0f;   // RUN (giữ Shift)
    m_acceleration    = 600.0f;   // tăng tốc cùng hướng
    m_deceleration    = 450.0f;   // ma sát khi thả phím (chậm hơn accel)
    m_skidDeceleration = 1800.0f; // phanh gấp khi bấm ngược (3x accel)
    m_airAcceleration = 120.0f;   // air control rất yếu
    m_jumpVelocity    = -380.0f;
    m_gravity         = 1200.0f;

    m_maxHealth   = 100;
    m_attackPower = 80;
    m_defense     = 10;
}

std::string Mario::getTexturePath() const {
    return "assets/texture/hero/mario.png";
}
