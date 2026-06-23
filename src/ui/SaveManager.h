#pragma once

#include "../interfaces/ISaveManager.h"

/**
 * @class SaveManager
 * @brief Stub implementation of save/load management
 * 
 * Developer: Nguyen Phuc
 * Status: STUB - Replace with full implementation
 * 
 * This stub provides minimal functionality to keep the project compilable.
 * Nguyen Phuc will implement full game state persistence here.
 */
class SaveManager : public ISaveManager
{
public:
    SaveManager();
    ~SaveManager() override = default;

    void initialize() override;
    bool saveGame() override;
    bool loadGame() override;
    bool hasSaveFile() const override;

private:
    bool m_hasSave;
};
