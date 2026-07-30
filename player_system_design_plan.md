# Kế hoạch Cải tiến Hệ thống Player (Mario Bros NES)
*Tích hợp Đa Nhân Vật (Mario, Luigi) và Đa Hình Thái (Normal, Super, Fire)*

---

## 1. Mục tiêu và Kiến trúc Tổng quan
- **Thay đổi tối thiểu:** Giữ nguyên vai trò Controller cốt lõi của `PlayerManager` và `IPlayerManager`.
- **Đa nhân vật (Characters):** Sử dụng **Inheritance (Kế thừa)**. `Mario` và `Luigi` kế thừa từ `PlayerManager` và ghi đè `setupStats()` để thiết lập chỉ số vật lý riêng biệt.
- **Đa hình thái (Forms):** Sử dụng **Strategy Pattern**. Tách logic về sprite, kích thước hitbox, và phản ứng với sát thương/item ra khỏi `PlayerManager` thông qua interface `IPlayerForm`.

---

## 2. Sơ đồ Lớp Cập nhật (UML)

```text
┌─────────────────────────────────────────────┐
│              PlayerManager                    │ (Chứa logic vòng lặp chính)
│                                             │
│ + setupStats() = 0 (Pure virtual)           │
│ - std::unique_ptr<IPlayerForm> m_currentForm│ <── Strategy Pattern Delegation
└──────────────────┬──────────────────────────┘
                   │ inherits
          ┌────────┴────────┐
          │                 │
┌─────────┴──────┐  ┌──────┴─────────┐
│     Mario      │  │     Luigi       │
│  setupStats()  │  │  setupStats()   │
│  (override)    │  │  (override)     │
└────────────────┘  └────────────────┘

┌─────────────────────────────────────────────┐
│               IPlayerForm                     │ (Interface xử lý hình thái)
│ + getWalkFrame1() / getWalkFrame2()         │
│ + getJumpFrame()                            │
│ + getHitboxSize()                           │
│ + takeDamage(PlayerManager& player)         │
│ + evolve(PlayerManager& player, ItemType t) │
└──────────────────┬──────────────────────────┘
                   │ implements
 ┌─────────────────┼─────────────────┐
 │                 │                 │
NormalForm     SuperForm          FireForm
```

---

## 3. Lộ trình Triển khai Chi tiết

### Bước 1: Xây dựng Hệ thống Form (Strategy Pattern)
Tạo thư mục mới `src/entities/player/forms/` chứa các class liên quan đến hình thái.

1. **Định nghĩa Interface `IPlayerForm`:** 
   Khai báo các hàm ảo thuần túy để lấy tọa độ sprite (thay vì namespace `HeroSprite` cứng), lấy kích thước hitbox, và các hàm phản ứng (`takeDamage`, `evolve`).
2. **Triển khai `NormalForm`, `SuperForm`, `FireForm`:**
   Mỗi class sẽ chứa các tham số về `IntRect` cho sprite. Để hỗ trợ cả Mario và Luigi dùng chung một class Form, constructor của các Form này sẽ nhận vào các thông số Rect tương ứng của nhân vật đó.

### Bước 2: Nâng cấp `PlayerManager`
Chỉnh sửa `PlayerManager.h` và `PlayerManager.cpp`:

1. **Thêm thuộc tính:** Bổ sung `std::unique_ptr<IPlayerForm> m_currentForm;`
2. **Cập nhật `updateAnimation(dt)`:**
   Thay vì lấy từ `HeroSprite::WalkFrame1`, hãy gọi `m_sprite->setTextureRect(m_currentForm->getWalkFrame1());`
3. **Cập nhật `takeDamage()`:**
   Xóa logic trừ máu (`m_currentHealth -= 10`) và gọi ủy quyền: `m_currentForm->takeDamage(*this);`
4. **Hỗ trợ chuyển đổi Form:**
   Thêm một public method `void setForm(std::unique_ptr<IPlayerForm> newForm)` vào `PlayerManager` để các form hiện tại có thể "thay" form mới vào Player.


### Bước 3: Refactor Tương tác với Item (ItemManager)
Xử lý các đoạn hardcode liên quan đến Item trong `ItemManager` hoặc hàm callback của `PlayerManager`:

- Các loại item như `Mushroom` hay `FireFlower` sẽ không gọi `collectCoin(1)` nữa.
- Thay vào đó, gọi một hàm mới trên `PlayerManager` là `player->collectPowerUp(ItemType type)`.
- Hàm `collectPowerUp` sẽ tiếp tục ủy quyền cho `m_currentForm->evolve(*this, type);`.

---
