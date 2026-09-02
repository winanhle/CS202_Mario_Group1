#pragma once
#include "PlayerManager.h"
#include "forms/FormOffsets.h"
#include <string>

/**
 * @class Luigi
 * @brief Luigi – nhảy cao hơn, trơn hơn Mario.
 *
 * Khai báo LUIGI_OFFSETS chứa tọa độ sprite cho 3 form (Normal/Super/Fire)
 * phù hợp với sprite sheet assets/texture/hero/luigi.png.
 */
class Luigi : public PlayerManager {
public:
    Luigi();
    void resetForm() override;

protected:
    void setupStats() override;
    std::string getTexturePath() const override;
};
