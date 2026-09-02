#pragma once
#include "PlayerManager.h"
#include "forms/FormOffsets.h"
#include <string>

/**
 * @class Mario
 * @brief Mario – stats nhanh, nhảy chuẩn.
 *
 * Khai báo MARIO_OFFSETS chứa tọa độ sprite cho 3 form (Normal/Super/Fire)
 * phù hợp với sprite sheet assets/texture/hero/mario.png.
 */
class Mario : public PlayerManager {
public:
    Mario();
    void resetForm() override;

protected:
    void setupStats() override;
    std::string getTexturePath() const override;
};
