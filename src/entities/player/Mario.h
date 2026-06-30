#pragma once
#include "PlayerManager.h"

class Mario : public PlayerManager {
protected:
    void setupStats() override;
};