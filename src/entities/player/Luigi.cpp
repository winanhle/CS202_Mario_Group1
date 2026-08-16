#include "Luigi.h"
#include "forms/NormalForm.h"

static const CharacterFormOffsets LUIGI_OFFSETS = {
    // normalForm  
    { {{1,  2}, {15, 23}}, {{18, 2}, {15, 23}}, {{52, 2}, {15, 23}}, {15.f, 23.f} },
    // superForm  
    { {{1, 27}, {15, 30}}, {{18, 27}, {15, 30}}, {{52, 27}, {15, 30}}, {15.f, 30.f} },
    // fireForm 
    { {{1, 60}, {15, 30}}, {{18, 60}, {15, 30}}, {{52, 60}, {15, 30}}, {15.f, 30.f} },
};

Luigi::Luigi() {
    m_currentForm = std::make_unique<NormalForm>(LUIGI_OFFSETS);
}

void Luigi::setupStats() {
    m_maxSpeed     = 190.0f;
    m_acceleration = 750.0f;
    m_friction     = 0.85f;
    m_jumpVelocity = -480.0f;
    m_gravity      = 1100.0f;

    m_maxHealth   = 80;
    m_attackPower = 90;
    m_defense     = 5;
}

std::string Luigi::getTexturePath() const {
    return "assets/texture/hero/luigi.png";
}
