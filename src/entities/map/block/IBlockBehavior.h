#pragma once

class IMapContext;    // narrow context interface — not the concrete MapManager
class IPlayerManager;

/**
 * @interface IBlockBehavior
 * @brief Strategy/State pattern cho từng loại block trong tilemap.
 *
 * Mỗi TileType có một behavior xác định:
 *  - Có solid hay không (va chạm).
 *  - Phản ứng khi player đập từ dưới lên (side-effects + chuyển trạng thái
 *    thông qua IMapContext::setTile để đồng bộ cả m_mapData lẫn m_rawGids).
 *
 * Nguyên tắc OOP:
 *  - SRP: hành vi từng block nằm trong từng subclass, MapManager chỉ dispatch.
 *  - OCP: thêm loại block mới = thêm subclass + đăng ký vào factory,
 *         không sửa MapManager.
 *  - DIP: MapManager phụ thuộc interface này; block behaviors phụ thuộc
 *         IMapContext (không phụ thuộc concrete MapManager).
 */
class IBlockBehavior
{
public:
    virtual ~IBlockBehavior() = default;

    /** @brief True nếu tile chặn di chuyển của player/enemy (mọi hướng). */
    virtual bool isSolid() const = 0;

    /**
     * @brief True nếu tile có thể bị đập từ dưới lên (player đang đi lên,
     *        velocityY < 0). Mặc định giống isSolid(); HIDDEN_BLOCK override
     *        thành true để bump được dù isSolid() là false (xuyên qua từ
     *        trái/phải/trên).
     */
    virtual bool isSolidFromBelow() const { return isSolid(); }

    /**
     * @brief Xử lý khi player đập block từ dưới lên.
     * @param ctx      Narrow map context (setTile + side-effects).
     * @param gx       Grid column của tile.
     * @param gy       Grid row của tile.
     * @param player   Player hiện tại (nullptr nếu không có).
     */
    virtual void onHitFromBelow(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const = 0;

    /**
     * @brief Xử lý khi player đứng trên mặt trên của block (grounded).
     * Mặc định không làm gì; PIPE_ENTRANCE override để kiểm tra phím Down và chuyển map.
     */
    virtual void onStandingOn(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const {
        (void)ctx; (void)gx; (void)gy; (void)player;
    }

    /**
     * @brief Xử lý khi player chạm cạnh bên của block.
     * Mặc định không làm gì; PIPE_EXIT override để kích hoạt warp quay lại stage chính.
     */
    virtual void onSideTouch(IMapContext& ctx, int gx, int gy, IPlayerManager* player) const {
        (void)ctx; (void)gx; (void)gy; (void)player;
    }
};
