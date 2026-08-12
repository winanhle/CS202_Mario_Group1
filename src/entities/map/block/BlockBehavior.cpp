#include "BlockBehavior.h"

#include <unordered_map>

// =============================================================================
//  Implementations — chuyển trạng thái qua MapManager::setTile để đảm bảo
//  m_mapData và m_rawGids (texture) LUÔN đồng bộ (Encapsulation).
// =============================================================================

void EmptyBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)map; (void)gx; (void)gy; (void)formType; // không phản ứng
}

void GroundBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)map; (void)gx; (void)gy; (void)formType; // không phản ứng
}

void PipeBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)map; (void)gx; (void)gy; (void)formType; // không phản ứng
}

void BrickBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)formType;
    map.spawnBrickDebris(gx, gy);               // mảnh gạch bay
    map.setTile(gx, gy, TileType::EMPTY);       // → EMPTY + load texture EMPTY
}

void QuestionCoinBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)formType;
    map.spawnCoinPop(gx, gy);                       // pop animation + award coin
    map.setTile(gx, gy, TileType::SOLID_BRICK);     // → block đã dùng (đổi texture)
}

void QuestionPowerupBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    map.spawnItemForFormType(gx, gy, formType); // Mushroom (Normal) / FireFlower (Super/Fire)
    map.setTile(gx, gy, TileType::SOLID_BRICK); // → block đã dùng (đổi texture)
}

// MULTI_COIN: vẫn cho coin trong cửa sổ 3.5s (award lặp lại ở mỗi lần đập),
// countdown do MapManager::update() xử lý → setTile(SOLID_BRICK) khi hết giờ.
void MultiCoinBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)formType;
    map.spawnCoinPop(gx, gy);          // pop animation + award coin
    map.setMultiCoinActive(gx, gy);    // bắt đầu / giữ countdown 3.5s
}

// HIDDEN_BLOCK: vô hình + không solid; khi đập → trở thành SOLID_BRICK.
void HiddenBlockBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)formType;
    map.setTile(gx, gy, TileType::SOLID_BRICK); // → solid + hiện texture block đã dùng
}

void SolidBrickBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)map; (void)gx; (void)gy; (void)formType; // không phản ứng
}

void DeathZoneBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)map; (void)gx; (void)gy; (void)formType; // không phản ứng
}

void FlagpoleBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)map; (void)gx; (void)gy; (void)formType; // không phản ứng
}

void CoinBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) const
{
    (void)map; (void)gx; (void)gy; (void)formType; // không phản ứng
}

// =============================================================================
//  Factory (Strategy lookup) — OCP: thêm block mới = thêm class + entry ở đây.
// =============================================================================

const IBlockBehavior& getBlockBehavior(TileType type)
{
    static const EmptyBehavior          empty;
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

    static const std::unordered_map<TileType, const IBlockBehavior*> table{
        {TileType::EMPTY,              &empty},
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
    };

    auto it = table.find(type);
    return (it != table.end()) ? *it->second : empty;
}
