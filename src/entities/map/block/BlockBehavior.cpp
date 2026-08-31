#include "../../../interfaces/IPlayerManager.h"
#include "../../../interfaces/IMapContext.h"
#include "BlockBehavior.h"

#include <unordered_map>

// =============================================================================
//  Implementations — chuyển trạng thái qua IMapContext::setTile để đảm bảo
//  m_mapData và m_rawGids (texture) LUÔN đồng bộ (Encapsulation).
//  Brick behavior cũng gọi killEnemiesAboveTile để diệt enemy ngồi trên đỉnh.
// =============================================================================

void EmptyBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

void BackgroundBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng (giống EMPTY)
}

void GroundBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

void PipeBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

// BRICK_NORMAL: Super/Fire Mario breaks it → kill enemies above → debris → EMPTY.
// Normal Mario: no effect (just bump animation handled by PlayerManager).
void BrickBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    if (player && player->getFormType() != FormType::Normal) {
        ctx.killEnemiesAboveTile(gx, gy); // diệt enemy đứng trên gạch vỡ
        ctx.spawnBrickDebris(gx, gy);
        ctx.setTile(gx, gy, TileType::EMPTY);
        player->addScore(50);
    }
}

// QUESTION_COIN: spawn coin pop → transition to QUESTION_USED (exhausted ? visual).
void QuestionCoinBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.spawnCoinPop(gx, gy);
    if (player) player->collectCoin(1);
    ctx.setTile(gx, gy, TileType::QUESTION_USED); // đổi sang texture ? đã dùng
}

// QUESTION_POWERUP: spawn item → transition to QUESTION_USED.
void QuestionPowerupBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.spawnItemForFormType(gx, gy, player ? static_cast<int>(player->getFormType()) : 0);
    ctx.setTile(gx, gy, TileType::QUESTION_USED); // đổi sang texture ? đã dùng
}

// MULTI_COIN: vẫn cho coin trong cửa sổ 3.5s (award lặp lại ở mỗi lần đập),
// countdown do MapManager::update() xử lý → setTile(QUESTION_USED) khi hết giờ.
void MultiCoinBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.spawnCoinPop(gx, gy);
    if (player) player->collectCoin(1);
    ctx.setMultiCoinActive(gx, gy);
}

// HIDDEN_BLOCK: vô hình + không solid; khi đập → trở thành SOLID_BRICK.
void HiddenBlockBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)player;
    ctx.setTile(gx, gy, TileType::SOLID_BRICK); // → solid + hiện texture block đã dùng
}

void SolidBrickBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

// QUESTION_USED: ? block đã cạn — không phản ứng, giống SOLID_BRICK.
void QuestionUsedBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

void DeathZoneBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

void FlagpoleBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

void CoinBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng
}

void FireBarBlockBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)ctx; (void)gx; (void)gy; (void)player; // không phản ứng (solid block)
}

// =============================================================================
//  Factory (Strategy lookup) — OCP: thêm block mới = thêm class + entry ở đây.
// =============================================================================

const IBlockBehavior& getBlockBehavior(TileType type)
{
    static const EmptyBehavior          empty;
    static const BackgroundBehavior     background;
    static const GroundBehavior         ground;
    static const PipeBehavior           pipe;
    static const BrickBehavior          brick;
    static const QuestionCoinBehavior   qCoin;
    static const QuestionPowerupBehavior qPower;
    static const MultiCoinBehavior      multiCoin;
    static const HiddenBlockBehavior    hidden;
    static const SolidBrickBehavior     solid;
    static const QuestionUsedBehavior   qUsed;
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
        {TileType::QUESTION_USED,      &qUsed},
        {TileType::DEATH_ZONE,         &death},
        {TileType::FLAGPOLE,           &flag},
        {TileType::COIN,               &coin},
        {TileType::FIRE_BAR,           &fireBar},
    };

    auto it = table.find(type);
    return (it != table.end()) ? *it->second : empty;
}
