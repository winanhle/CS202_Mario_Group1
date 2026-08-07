#pragma once
#include "PlayerManager.h"

class Mario : public PlayerManager {
public:
    Mario();
protected:
    void setupStats() override;
};
