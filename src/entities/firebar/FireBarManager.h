#pragma once

#include "../../interfaces/IFireBarManager.h"
#include "../../interfaces/IPlayerManager.h"
#include "FireBar.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

/**
 * @class FireBarManager
 * @brief Manages all FireBar obstacle instances for the current stage.
 *
 * Responsibilities:
 *   - Loads and owns the shared fireball texture on initialize()
 *   - Spawns FireBar instances from map data
 *   - Advances rotation and animation every frame
 *   - Resolves collision against active player(s) and applies damage/death
 *   - Renders all FireBars
 */
class FireBarManager : public IFireBarManager
{
public:
    FireBarManager() = default;
    ~FireBarManager() override = default;

    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    void setPlayerManager(IPlayerManager* player) override { m_player = player; }
    void setPlayerManager2(IPlayerManager* player) override { m_player2 = player; }

    void spawnFromMapData(const std::vector<FireBarSpawnData>& spawns) override;

    int getFireBarCount() const { return static_cast<int>(m_firebars.size()); }

private:
    IPlayerManager* m_player  = nullptr;
    IPlayerManager* m_player2 = nullptr;

    sf::Texture m_fireballTexture;
    std::vector<std::unique_ptr<FireBar>> m_firebars;

    void resolvePlayerCollision(IPlayerManager* player);
};
