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

class PipeBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const override;
};

class BrickBehavior final : public IBlockBehavior {
public:
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

class HiddenBlockBehavior final : public IBlockBehavior {
public:
    // HIDDEN_BLOCK: vô hình & KHÔNG solid từ trái/phải/trên (Mario đi xuyên qua).
    // Chỉ bump được từ dưới lên (isSolidFromBelow = true) vì PlayerManager
    // gọi onHitFromBelow trong nhánh isSolidFromBelow(). Sau khi đập →
    // setTile(SOLID_BRICK) trở thành block solid thật sự.
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
