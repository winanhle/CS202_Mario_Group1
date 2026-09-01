#pragma once

#include "IBlockBehavior.h"
#include "../../../interfaces/IMapContext.h"

// =============================================================================
//  Concrete behaviors — mỗi loại block một subclass (OCP).
//  Thêm loại block mới = thêm class + đăng ký trong getBlockBehavior().
//  Mọi onHitFromBelow nhận IMapContext& (không phụ thuộc concrete MapManager — DIP).
// =============================================================================

class EmptyBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class BackgroundBehavior final : public IBlockBehavior {
public:
    // BACKGROUND (bụi cỏ, đám mây trang trí) giống hệt EMPTY:
    // không solid, không phản ứng khi bị đập.
    bool isSolid() const override { return false; }
    bool isSolidFromBelow() const override { return false; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class GroundBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class PipeBehavior : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class PipeEntranceBehavior final : public PipeBehavior {
public:
    void onStandingOn(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class PipeExitBehavior final : public PipeBehavior {
public:
    void onSideTouch(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
    void onStandingOn(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class BrickEmptyBehavior final : public IBlockBehavior {
public:
    // BRICK_EMPTY (BRICK_NORMAL): Khi player Super/Fire đập -> vỡ vụn thành EMPTY.
    // Khi player Normal đập -> nảy lên nửa ô rồi về vị trí cũ (không vỡ).
    // Đều diệt/đẩy enemy đứng trên gạch.
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};
using BrickBehavior = BrickEmptyBehavior; // Alias for backward compatibility

class BrickSolidBehavior final : public IBlockBehavior {
public:
    // BRICK_SOLID: Khi đập -> nảy lên nửa ô và biến thành QUESTION_USED.
    // Diệt/đẩy enemy đứng trên gạch.
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class QuestionCoinBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class QuestionPowerupBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class MultiCoinBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class MultiCoin2Behavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class HiddenBlockBehavior final : public IBlockBehavior {
    // HIDDEN_BLOCK: vô hình & KHÔNG solid từ trái/phải/trên (Mario đi xuyên qua).
    // Chỉ bump được từ dưới lên (isSolidFromBelow = true) vì PlayerManager
    // gọi onHitFromBelow trong nhánh isSolidFromBelow(). Sau khi đập →
    // setTile(QUESTION_USED) trở thành block solid thật sự.
public:
    bool isSolid() const override { return false; }
    bool isSolidFromBelow() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class SolidBrickBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class QuestionUsedBehavior final : public IBlockBehavior {
public:
    // QUESTION_USED: ? block đã dùng hết — solid, không phản ứng khi đập.
    // Texture riêng (khác SOLID_BRICK) được load qua m_typeToGid[QUESTION_USED].
    // Để sử dụng: tag tile tương ứng trong .tsx với name="type" value="QUESTION_USED".
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class DeathZoneBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class FlagpoleBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class CoinBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class FireBarBlockBehavior final : public IBlockBehavior {
public:
    // FIRE_BAR block là solid để Mario có thể đứng/chạy lên trên
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

// ─── Factory ──────────────────────────────────────────────────────────────────
// Trả về behavior tương ứng với TileType. Singleton tĩnh — không quản lý ownership.
const IBlockBehavior& getBlockBehavior(TileType type);
