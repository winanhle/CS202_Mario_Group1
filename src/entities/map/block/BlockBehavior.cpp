#include "../../../interfaces/IPlayerManager.h"
#include "BlockBehavior.h"

#include <unordered_map>

// =============================================================================
//  Implementations — chuyển trạng thái qua MapManager::setTile để đảm bảo
//  m_mapData và m_rawGids (texture) LUÔN đồng bộ (Encapsulation).
// =============================================================================

void EmptyBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng
}

void BackgroundBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng (giống EMPTY)
}

void GroundBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng
}

void PipeBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng
}

void BrickBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    if (player && player->getFormType() != FormType::Normal) { // Super or Fire
        map.spawnBrickDebris(gx, gy);
        map.setTile(gx, gy, TileType::EMPTY);
        player->addScore(50);
    }
}

void QuestionCoinBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    map.spawnCoinPop(gx, gy);
    if (player) player->collectCoin(1);
    map.setTile(gx, gy, TileType::SOLID_BRICK);
}

void QuestionPowerupBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    map.spawnItemForFormType(gx, gy, player ? static_cast<int>(player->getFormType()) : 0);
    map.setTile(gx, gy, TileType::SOLID_BRICK);
}

// MULTI_COIN: vẫn cho coin trong cửa sổ 3.5s (award lặp lại ở mỗi lần đập),
// countdown do MapManager::update() xử lý → setTile(SOLID_BRICK) khi hết giờ.
void MultiCoinBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    map.spawnCoinPop(gx, gy);
    if (player) player->collectCoin(1);
    map.setMultiCoinActive(gx, gy);
}

// HIDDEN_BLOCK: vô hình + không solid; khi đập → trở thành SOLID_BRICK.
void HiddenBlockBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)player;
    map.setTile(gx, gy, TileType::SOLID_BRICK); // → solid + hiện texture block đã dùng
}

void SolidBrickBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng
}

void DeathZoneBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng
}

void FlagpoleBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng
}

void CoinBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng
}

void FireBarBlockBehavior::onHitFromBelow(MapManager& map, int gx, int gy, IPlayerManager* player) const
{
    (void)map; (void)gx; (void)gy; (void)player; // không phản ứng (solid block)
}

// =============================================================================
//  Factory (Strategy lookup) — OCP: thêm block mới = thêm class + entry ở đây.
// =============================================================================

const IBlockBehavior& getBlockBehavior(TileType type)
{
    static const EmptyBehavior          empty;
    static const BackgroundBehavior    background;
    static const GroundBehavior         ground;
    static const PipeBehavior           pipe;
    static const BrickBehavior          brick;
    static const QuestionCoinBehavior   qCoin;
    static const QuestionPowerupBehavior qPower;
    static const MultiCoinBehavior      multiCoin;
    static const HiddenBlockBehavior    hidden;
    static const SolidBrickBehavior     solid;
    static const DeathZoneBehavior      death;
    static const FlagpoleBehavior       flag;
    static const CoinBehavior           coin;
    static const FireBarBlockBehavior   fireBar;

    static const std::unordered_map<TileType, const IBlockBehavior*> table{
        {TileType::EMPTY,              &empty},
        {TileType::BACKGROUND,         &background},
        {TileType::GROUND,             &ground},
        {TileType::PIPE,               &pipe},
        {TileType::BRICK_NORMAL,       &brick},
        {TileType::QUESTION_COIN,      &qCoin},
        {TileType::QUESTION_POWERUP,   &qPower},
        {TileType::MULTI_COIN,         &multiCoin},
        {TileType::HIDDEN_BLOCK,       &hidden},
        {TileType::SOLID_BRICK,        &solid},
        {TileType::DEATH_ZONE,         &death},
        {TileType::FLAGPOLE,           &flag},
        {TileType::COIN,               &coin},
        {TileType::FIRE_BAR,           &fireBar},
    };

    auto it = table.find(type);
    return (it != table.end()) ? *it->second : empty;
}
