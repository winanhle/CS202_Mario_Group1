#pragma once

class MapManager;

/**
 * @interface IBlockBehavior
 * @brief Strategy/State pattern cho từng loại block trong tilemap.
 *
 * Mỗi TileType có một behavior xác định:
 *  - Có solid hay không (va chạm).
 *  - Phản ứng khi player đập từ dưới lên (side-effects + chuyển trạng thái
 *    thông qua MapManager::setTile để đồng bộ cả m_mapData lẫn m_rawGids).
 *
 * Nguyên tắc OOP:
 *  - SRP: hành vi từng block nằm trong từng subclass, MapManager chỉ dispatch.
 *  - OCP: thêm loại block mới = thêm subclass + đăng ký vào factory,
 *         không sửa MapManager.
 *  - DIP: MapManager phụ thuộc interface này, không phụ thuộc concrete class.
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
     * @param map      MapManager (để gọi setTile + side-effects).
     * @param gx       Grid column của tile.
     * @param gy       Grid row của tile.
     * @param formType Form hiện tại của player (Normal=0, Super/Fire>0),
     *                 dùng để quyết định Mushroom hay FireFlower.
     */
    virtual void onHitFromBelow(MapManager& map, int gx, int gy, int formType) const = 0;
};
