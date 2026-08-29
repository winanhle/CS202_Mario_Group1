#pragma once

#include "IBlockBehavior.h"
#include "../MapManager.h"

// =============================================================================
//  Concrete behaviors — mỗi loại block một subclass (OCP).
//  Thêm loại block mới = thêm class + đăng ký trong getBlockBehavior().
// =============================================================================

class EmptyBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class BackgroundBehavior final : public IBlockBehavior {
public:
    // BACKGROUND (bụi cỏ, đám mây trang trí) giống hệt EMPTY:
    // không solid, không phản ứng khi bị đập.
    bool isSolid() const override { return false; }
    bool isSolidFromBelow() const override { return false; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class GroundBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class PipeBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class BrickBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class QuestionCoinBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class QuestionPowerupBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class MultiCoinBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class HiddenBlockBehavior final : public IBlockBehavior {
public:
    // HIDDEN_BLOCK: vô hình & KHÔNG solid từ trái/phải/trên (Mario đi xuyên qua).
    // Chỉ bump được từ dưới lên (isSolidFromBelow = true) vì PlayerManager
    // gọi onHitFromBelow trong nhánh isSolidFromBelow(). Sau khi đập →
    // setTile(SOLID_BRICK) trở thành block solid thật sự.
    bool isSolid() const override { return false; }
    bool isSolidFromBelow() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class SolidBrickBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class DeathZoneBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class FlagpoleBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class CoinBehavior final : public IBlockBehavior {
public:
    bool isSolid() const override { return false; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

class FireBarBlockBehavior final : public IBlockBehavior {
public:
    // FIRE_BAR block là solid để Mario có thể đứng/chạy lên trên
    bool isSolid() const override { return true; }
    void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const override;
};

// ─── Factory ──────────────────────────────────────────────────────────────────
// Trả về behavior tương ứng với TileType. Singleton tĩnh — không quản lý ownership.
const IBlockBehavior& getBlockBehavior(TileType type);
