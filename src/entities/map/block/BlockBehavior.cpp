#include "../../../interfaces/IPlayerManager.h"
#include "../../../interfaces/IMapContext.h"
#include "BlockBehavior.h"

#include <unordered_map>

// =============================================================================
//  Implementations — chuyển trạng thái qua IMapContext::setTile hoặc spawnBlockBump
//  để đảm bảo m_mapData và m_rawGids (texture) LUÔN đồng bộ (Encapsulation).
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

void PipeEntranceBehavior::onStandingOn(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)gx; (void)gy;
    if (player && player->isDownPressed()) {
        ctx.requestWarp("assets/map/stage1_hidden.tmx");
    }
}

void PipeExitBehavior::onSideTouch(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)gx; (void)gy; (void)player;
    ctx.requestWarp("assets/map/stage1.tmx", 2616.f, 192.f);
}

void PipeExitBehavior::onStandingOn(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)gx; (void)gy; (void)player;
    ctx.requestWarp("assets/map/stage1.tmx", 2616.f, 192.f);
}

// BRICK_EMPTY (BRICK_NORMAL):
// - Super/Fire Mario: diệt enemy trên gạch -> tạo mảnh vỡ -> biến thành EMPTY.
// - Normal Mario: diệt enemy trên gạch -> nảy lên nửa ô (spawnBlockBump) và giữ nguyên BRICK_EMPTY.
void BrickEmptyBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.killEnemiesAboveTile(gx, gy); // diệt enemy đứng trên gạch vỡ/bị nảy
    if (player && player->getFormType() != FormType::Normal) {
        ctx.spawnBrickDebris(gx, gy);
        ctx.setTile(gx, gy, TileType::EMPTY);
        player->addScore(50);
    } else {
        ctx.spawnBlockBump(gx, gy, TileType::BRICK_EMPTY);
    }
}

// BRICK_SOLID:
// - Khi đập: diệt enemy trên gạch -> nảy lên nửa ô và biến thành QUESTION_USED.
void BrickSolidBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.killEnemiesAboveTile(gx, gy); // diệt enemy đứng trên gạch
    ctx.spawnBlockBump(gx, gy, TileType::QUESTION_USED);
    if (player) player->addScore(50);
}

// QUESTION_COIN:
// - Diệt enemy trên block -> nảy lên nửa ô -> spawn coin pop -> chuyển thành QUESTION_USED.
void QuestionCoinBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.killEnemiesAboveTile(gx, gy);
    ctx.spawnCoinPop(gx, gy);
    if (player) player->collectCoin(1);
    ctx.spawnBlockBump(gx, gy, TileType::QUESTION_USED); // nảy lên nửa ô và thành QUESTION_USED
}

// QUESTION_POWERUP:
// - Diệt enemy trên block -> spawn item -> nảy lên nửa ô -> chuyển thành QUESTION_USED.
void QuestionPowerupBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.killEnemiesAboveTile(gx, gy);
    ctx.spawnItemForFormType(gx, gy, player ? static_cast<int>(player->getFormType()) : 0);
    ctx.spawnBlockBump(gx, gy, TileType::QUESTION_USED); // nảy lên nửa ô và thành QUESTION_USED
}

// MULTI_COIN:
// - Vẫn cho coin và nảy lên trong cửa sổ 3.5s; countdown do MapManager::update() xử lý.
void MultiCoinBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.killEnemiesAboveTile(gx, gy);
    ctx.spawnCoinPop(gx, gy);
    if (player) player->collectCoin(1);
    ctx.spawnBlockBump(gx, gy, TileType::MULTI_COIN);
    ctx.setMultiCoinActive(gx, gy);
}

// MULTI_COIN2:
// - Frame 2 của MULTI_COIN khi đang hoạt động, có cùng hành vi khi bị đập tiếp
void MultiCoin2Behavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    ctx.killEnemiesAboveTile(gx, gy);
    ctx.spawnCoinPop(gx, gy);
    if (player) player->collectCoin(1);
    ctx.spawnBlockBump(gx, gy, TileType::MULTI_COIN);
    ctx.setMultiCoinActive(gx, gy);
}

// HIDDEN_BLOCK: vô hình + không solid; khi đập → nảy lên và trở thành SOLID_BRICK.
void HiddenBlockBehavior::onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const
{
    (void)player;
    ctx.killEnemiesAboveTile(gx, gy);
    ctx.spawnBlockBump(gx, gy, TileType::SOLID_BRICK);
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
    static const PipeEntranceBehavior   pipeEntrance;
    static const PipeExitBehavior       pipeExit;
    static const BrickEmptyBehavior     brickEmpty;
    static const BrickSolidBehavior     brickSolid;
    static const QuestionCoinBehavior   qCoin;
    static const QuestionPowerupBehavior qPower;
    static const MultiCoinBehavior      multiCoin;
    static const MultiCoin2Behavior     multiCoin2;
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
        {TileType::PIPE_ENTRANCE,      &pipeEntrance},
        {TileType::PIPE_EXIT,          &pipeExit},
        {TileType::BRICK_NORMAL,       &brickEmpty},
        {TileType::BRICK_EMPTY,        &brickEmpty},
        {TileType::BRICK_SOLID,        &brickSolid},
        {TileType::QUESTION_COIN,      &qCoin},
        {TileType::QUESTION_POWERUP,   &qPower},
        {TileType::MULTI_COIN,         &multiCoin},
        {TileType::MULTI_COIN2,        &multiCoin2},
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
