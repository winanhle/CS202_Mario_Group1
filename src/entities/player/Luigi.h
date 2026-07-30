#pragma once
#include "PlayerManager.h"

class Luigi : public PlayerManager {
public:
    Luigi();
protected:
    void setupStats() override;
};
