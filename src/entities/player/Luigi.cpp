#include "Luigi.h"
#include "forms/NormalForm.h"

static const CharacterFormOffsets LUIGI_OFFSETS = {
    // normalForm  
    { {{1,  2}, {16, 23}}, {{18, 2}, {16, 23}}, {{52, 2}, {16, 23}}, {{154, 1}, {16, 24}}, {15.f, 23.f} },
    // superForm  
    { {{1, 27}, {16, 30}}, {{18, 27}, {16, 30}}, {{52, 27}, {16, 30}}, {{154, 1}, {16, 24}}, {15.f, 30.f} },
    // fireForm 
    { {{1, 60}, {16, 30}}, {{18, 60}, {16, 30}}, {{52, 60}, {16, 30}}, {{154, 1}, {16, 24}}, {15.f, 30.f} },
};

Luigi::Luigi() {
    m_currentForm = std::make_unique<NormalForm>(LUIGI_OFFSETS);
}

void Luigi::setupStats() {
    m_maxSpeed        = 180.0f;   // WALK
    m_runMaxSpeed     = 310.0f;   // RUN (giữ Shift)
    m_acceleration    = 560.0f;   // tăng tốc cùng hướng
    m_deceleration    = 420.0f;   // ma sát khi thả phím (chậm hơn accel)
    m_skidDeceleration = 1700.0f; // phanh gấp khi bấm ngược (3x accel)
    m_airAcceleration = 110.0f;   // air control rất yếu
    m_jumpVelocity    = -400.0f;
    m_gravity         = 1100.0f;

    m_maxHealth   = 80;
    m_attackPower = 90;
    m_defense     = 5;
}

std::string Luigi::getTexturePath() const {
    return "assets/texture/hero/luigi.png";
}
