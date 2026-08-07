# CameraManager — Kế hoạch Kiến trúc

## 1. Vị trí module trong hệ thống

```
src/
  interfaces/
    ICameraManager.h            # [MỚI] Interface contract
  entities/
    camera/
      CameraManager.h           # [MỚI] Concrete class header
      CameraManager.cpp         # [MỚI] Implementation
  world/
    GameWorld.h                 # [SỬA] Thêm setCameraManager + getCameraManager
    GameWorld.cpp               # [SỬA] Thêm camera::update trong vòng update()
```

Module được xem là một "manager" độc lập, cùng cấp với PlayerManager, EnemyManager, MapManager. CameraManager **đọc** vị trí player — không can thiệp vật lý — và tự cập nhật `sf::View` riêng của nó.

---

## 2. Lớp và Trách nhiệm

### 2.1. `ICameraManager` (Interface)

Nằm ở `src/interfaces/ICameraManager.h`.

```cpp
class ICameraManager {
public:
    virtual ~ICameraManager() = default;
    virtual void initialize(sf::Vector2u mapSizePixels) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void setFollowTarget(const IPlayerManager* player) = 0;
    virtual sf::View getView() const = 0;
};
```

**Phân tích:**

| Method | Mục đích |
|--------|----------|
| `initialize(mapSizePixels)` | Nhận kích thước bản đồ (từ MapManager) — tính MapWidth dùng cho clamp |
| `update(deltaTime)` | Mỗi frame: tính target X/Y → lerp → clamp → apply vào m_view |
| `setFollowTarget(player)` | Nhận con trỏ `const IPlayerManager*` — Camera chỉ đọc hitbox, không sửa player |
| `getView()` | Trả về `sf::View` hiện tại để PlayState/GameWorld set cho window |

### 2.2. `CameraManager` (Concrete)

Nằm ở `src/entities/camera/CameraManager.h` và `.cpp`.

#### State (thuộc tính private)

```
sf::View m_view;                    // View hiện tại (camera position)
const IPlayerManager* m_targetPlayer; // Con trỏ const đến IPlayerManager (read-only)

// Deadzone
float m_deadzoneWidth;              // = m_viewWidth * 0.1 (VD 10% màn)
float m_viewWidth;                  // Kích thước màn ngang (VD 800)
float m_viewHeight;                 // Kích thước màn dọc (VD 600)

// Lerp speed
float m_lerpSpeedX;                 // Tốc độ bám X (VD 5.0)
float m_lerpSpeedY;                 // Tốc độ bám Y (VD 3.0)

// Clamping
float m_mapWidth;                   // Giới hạn X tối đa (pixels)

// Y-snapping
float m_snapThreshold;              // Ngưỡng chênh lệch Y mới snap (VD 16.0)
```

#### Logic update từng tính năng

**2.2.0. Lấy hitbox player qua interface**

```
// Đầu update(): query hitbox từ interface
sf::FloatRect targetBox = m_targetPlayer->getHitbox();
// Mọi tính toán sau dùng targetBox (chứ không lưu con trỏ hitbox)
```

**2.2.1. Deadzone (X)**

```
Mỗi frame, từ targetBox → playerCenterX
deadzoneLeft  = m_view.getCenter().x - m_deadzoneWidth / 2
deadzoneRight = m_view.getCenter().x + m_deadzoneWidth / 2

Nếu playerCenterX < deadzoneLeft:
    targetX = playerCenterX + m_deadzoneWidth / 2
Nếu playerCenterX > deadzoneRight:
    targetX = playerCenterX - m_deadzoneWidth / 2
Nếu playerCenterX nằm trong (deadzoneLeft, deadzoneRight):
    targetX = m_view.getCenter().x (giữ nguyên)
```

**2.2.2. Y-Axis Lock + Platform Snapping (Y)**

```
playerCenterY = targetBox.top + targetBox.height / 2
diffY = abs(playerCenterY - m_view.getCenter().y)

Nếu diffY > m_snapThreshold:
    // Player thực sự lên nền cao hơn / rớt xuống vực
    targetY = playerCenterY
Ngược lại:
    targetY = m_view.getCenter().y (giữ nguyên Y)
```

**2.2.3. Smooth Damping (Lerp)**

```
// Lerp frame-independent (dùng công thức chuẩn SFML)
m_view.setCenter(
    currentCenter.x + (targetX - currentCenter.x) * (1 - exp(-m_lerpSpeedX * deltaTime)),
    currentCenter.y + (targetY - currentCenter.y) * (1 - exp(-m_lerpSpeedY * deltaTime))
);
```

**2.2.4. Map Clamping (giới hạn)**

```
// Sau lerp — clamp X
viewCenterX = m_view.getCenter().x
halfW = m_view.getSize().x / 2

// clampX sao cho view.left >= 0 && view.right <= m_mapWidth
clampedX = clamp(viewCenterX, halfW, m_mapWidth - halfW)

// Y có thể không clamp (nếu map mở rộng) hoặc clamp nếu có giới hạn dọc
```

---

## 3. Luồng tích hợp vào GameWorld::update()

Thứ tự trong `GameWorld::update(float deltaTime)`:

```
// === Các bước hiện tại — GIỮ NGUYÊN ===
1. mapManager->update(deltaTime)
2. playerManager->update(deltaTime)     → Player di chuyển, vị trí FINAL
3. enemyManager->update(deltaTime)
4. itemManager->update(deltaTime)
5. hudManager->update(deltaTime)

// === BƯỚC MỚI: Camera ===
6. cameraManager->update(deltaTime)     → Camera đọc vị trí player, lerp, clamp
```

**Luồng dữ liệu trong bước 6:**

```
GameWorld::update()
  │
  ├─ cameraManager->update(dt)
  │    │
  │    ├─ Gọi m_targetPlayer->getHitbox() → sf::FloatRect  // READ-ONLY qua interface
  │    ├─ Tính targetX / targetY (deadzone + Y-lock)
  │    ├─ Lerp m_view về target
  │    ├─ Clamp m_view trong [0, m_mapWidth]
  │    └─ (không modify player — m_targetPlayer là const*)
  │
  └─ (kết thúc camera update)
```

**Render:** PlayState gọi `window.setView(cameraManager->getView())` *trước* khi render cảnh nhằm áp dụng camera transform.

```
GameWorld::render(window)
  │
  ├─ [MỚI] window.setView(cameraManager->getView())
  ├─ mapManager->render(window)
  ├─ enemyManager->render(window)
  ├─ itemManager->render(window)
  ├─ playerManager->render(window)
  │
  └─ [HUD] window.setView(window.getDefaultView())
      └─ hudManager->render(window)
```

---

## 4. Các quyết định thiết kế

### 4.1. Tại sao là `const IPlayerManager*` thay vì `const sf::FloatRect*`?

- CameraManager không được sửa bất kỳ thứ gì của player → `const IPlayerManager*` đảm bảo **toàn bộ interface** là read-only (không thể gọi `takeDamage()`, `setPosition()`, v.v.)
- CameraManager tự query hitbox qua `m_targetPlayer->getHitbox()` mỗi frame — hitbox luôn fresh, không lo con trỏ FloatRect bị invalidate sau khi player update
- Nếu sau này Camera cần thêm thông tin từ player (velocity, state, v.v.), `IPlayerManager*` sẵn sàng cung cấp mà không cần sửa interface Camera
- Raw pointer (không shared) vì CameraManager không sở hữu player — chỉ tham chiếu
- Quy tắc: Camera chỉ **đọc**, không bao giờ **ghi** vào player

### 4.2. Tại sao dùng `exp(-k * dt)` thay vì `lerp(a, b, t)` đơn giản?

- Lerp `t` thường là `speed * dt` — nếu dt biến thiên thì tốc độ không đều
- `1 - exp(-k * dt)` là frame-independent: cho tốc độ ổn định ở mọi fps
- Công thức chuẩn ngành game cho camera mượt

### 4.3. Tham số "ma thuật" (magic numbers) cần expose?

Để đơn giản hóa phase 1, các tham số sau có thể là hằng số `constexpr` trong `.cpp`:

| Tham số | Giá trị gợi ý | Ghi chú |
|---------|---------------|---------|
| `m_deadzoneWidth` | `m_viewWidth * 0.1f` | 10% màn hình |
| `m_lerpSpeedX` | `5.0f` | Bám ngang nhanh |
| `m_lerpSpeedY` | `3.0f` | Bám dọc chậm hơn |
| `m_snapThreshold` | `16.0f` | ~1 tile |
| `m_viewWidth` | `800.0f` | Khớp window |
| `m_viewHeight` | `600.0f` | Khớp window |

Về sau có thể config qua file hoặc setter nếu cần tuning.

### 4.4. Tại sao view size cứng?

Trong kiến trúc Mario platformer, màn hình là cố định (800×600). View size khớp với window size. Nếu sau này muốn zoom, có thể thay đổi `m_view.setSize(...)`.

---

## 5. Danh sách thay đổi file cụ thể

### File 1: `src/interfaces/ICameraManager.h` **[TẠO MỚI]**

```
- pragma once
- forward declaration class IPlayerManager (để tránh include chéo)
- include <SFML/Graphics/View.hpp>, <SFML/System/Vector2.hpp>
- class ICameraManager
  - virtual ~ICameraManager() = default
  - virtual void initialize(sf::Vector2u mapSizePixels) = 0
  - virtual void update(float deltaTime) = 0
  - virtual void setFollowTarget(const IPlayerManager* player) = 0
  - virtual sf::View getView() const = 0
```

### File 2: `src/entities/camera/CameraManager.h` **[TẠO MỚI]**

```
- #pragma once
- #include "../../interfaces/ICameraManager.h"
- class CameraManager : public ICameraManager
  - (các hàm override)
  - private members như mục 2.2
  - Lưu ý: m_targetPlayer là const IPlayerManager*, không phải FloatRect*
```

### File 3: `src/entities/camera/CameraManager.cpp` **[TẠO MỚI]**

```
- #include "../../interfaces/IPlayerManager.h"  (để gọi getHitbox())
- Implement toàn bộ logic:
  - initialize(): cài đặt view, deadzone, lerp speed, mapWidth
  - setFollowTarget(): gán m_targetPlayer
  - update():
      1. sf::FloatRect box = m_targetPlayer->getHitbox()
      2. deadzone X (dùng box)
      3. Y-lock + snap (dùng box)
      4. exp-lerp
      5. map clamp
  - getView(): return m_view
```

### File 4: `src/world/GameWorld.h` **[SỬA]**

```
- Thêm forward declaration class ICameraManager
- Thêm members:
  - std::shared_ptr<ICameraManager> m_cameraManager
- Thêm public methods:
  - void setCameraManager(std::shared_ptr<ICameraManager>)
  - ICameraManager* getCameraManager()
- Thêm trong injectDependencies():
  - cameraManager->setFollowTarget(m_playerManager.get())
  // Truyền IPlayerManager* (const), Camera tự gọi getHitbox()
```

### File 5: `src/world/GameWorld.cpp` **[SỬA]**

```
- Thêm #include "../interfaces/ICameraManager.h"
- Trong initialize():
  - Nếu m_cameraManager:
      - m_cameraManager->initialize(m_mapManager?...)
- Trong injectDependencies():
  - Nếu m_cameraManager && m_playerManager:
      - m_cameraManager->setFollowTarget(m_playerManager.get())
      // Truyền IPlayerManager* (const), CameraManager tự query hitbox
- Trong update() — step 6 (sau hudManager update):
  - Nếu m_cameraManager:
      - m_cameraManager->update(deltaTime)
- Trong render() — đầu hàm, trước map render:
  - Nếu m_cameraManager:
      - window.setView(m_cameraManager->getView())
```

### File 6: `CMakeLists.txt` **[SỬA]**

```
- Thêm src/entities/camera/CameraManager.cpp vào cả hai target
```

---

## 6. Sơ đồ lớp quan hệ

```
┌──────────────────────┐          ┌──────────────────────────────┐
│   GameWorld          │──────────│  ICameraManager (interface)   │
│                      │  owns    │                              │
│  ┌────────────────┐  │  ┌───────│+ initialize(mapSize)         │
│  │ m_playerManager │  │  │       │+ update(dt)                  │
│  │ (IPlayerManager*)│  │  │       │+ setFollowTarget(player*)   │
│  └────────┬───────┘  │  │       │+ getView(): sf::View         │
│           │          │  │       └──────────┬───────────────────┘
│  injectDependencies(): │                   │implements
│  camera->setFollow-   │         ┌──────────┴────────────────────┐
│  Target(player)       │         │ CameraManager (concrete)      │
│  (const IPlayerMgr*)  │         │                               │
└──────────────────────┘          │- m_view: sf::View            │
                                  │- m_targetPlayer: const        │
                                  │    IPlayerManager*            │
                                  │- m_deadzoneWidth: float       │
                                  │- m_mapWidth: float            │
                                  │- m_lerpSpeedX/Y: float        │
                                  │- m_snapThreshold: float       │
                                  │                               │
                                  │- update():                     │
                                  │   1. box=m_targetPlayer->     │
                                  │       getHitbox()          │
                                  │   2. deadzone X (dùng box) │
                                  │   3. Y-lock + snap (dùng box)│
                                  │   4. exp-lerp                 │
                                  │   5. map clamp                │
                                  └───────────────────────────────┘
```

---

## 7. Câu hỏi mở / cần quyết định

1. **Camera cần get MapWidth từ đâu?**
   - MapManager nên có method `getPixelWidth()` (hoặc `getColumnCount() * tileSize`) đã expose qua IMapManager → GameWorld truyền vào `cameraManager->initialize(vu)`.
   - Giải pháp: Thêm `virtual sf::Vector2u getMapPixelSize() const = 0;` vào `IMapManager.h`.

2. **Y-clamping có cần bound dưới?**
   - Nếu map có chiều cao cố định, nên clamp Y tương tự X để tránh camera chiếu vào vùng đen.

3. **Có cần hỗ trợ camera shake / transition không?**
   - Phase 1: không. Để dành extension point: `virtual void shake(float intensity, float duration) = 0;` có thể thêm sau.

---

## 8. Ví dụ code trực quan cho phần core (chỉ minh họa)

```cpp
// CameraManager::update(float dt)
void CameraManager::update(float dt) {
    if (!m_targetPlayer) return;

    // Lấy hitbox từ interface — READ-ONLY, không sửa player
    sf::FloatRect box = m_targetPlayer->getHitbox();

    // --- Deadzone X ---
    float playerCenterX = box.left + box.width / 2.0f;
    float deadLeft = m_view.getCenter().x - m_deadzoneWidth / 2.0f;
    float deadRight = m_view.getCenter().x + m_deadzoneWidth / 2.0f;

    float targetX = m_view.getCenter().x;
    if (playerCenterX < deadLeft)
        targetX = playerCenterX + m_deadzoneWidth / 2.0f;
    else if (playerCenterX > deadRight)
        targetX = playerCenterX - m_deadzoneWidth / 2.0f;

    // --- Y-lock + Snap ---
    float playerCenterY = box.top + box.height / 2.0f;
    float targetY = m_view.getCenter().y;
    if (std::abs(playerCenterY - m_view.getCenter().y) > m_snapThreshold)
        targetY = playerCenterY;

    // --- Lerp frame-independent ---
    float alphaX = 1.0f - std::exp(-m_lerpSpeedX * dt);
    float alphaY = 1.0f - std::exp(-m_lerpSpeedY * dt);
    sf::Vector2f center = m_view.getCenter();
    center.x += (targetX - center.x) * alphaX;
    center.y += (targetY - center.y) * alphaY;

    // --- Map Clamping X ---
    float halfW = m_view.getSize().x / 2.0f;
    center.x = std::clamp(center.x, halfW, m_mapWidth - halfW);

    m_view.setCenter(center);
}
```

---

## 9. Thứ tự implementation

| Step | Task | File(s) |
|------|------|---------|
| 1 | Tạo ICameraManager interface | `src/interfaces/ICameraManager.h` |
| 2 | Tạo CameraManager (h + cpp) | `src/entities/camera/CameraManager.h/.cpp` |
| 3 | Sửa GameWorld: header + cpp | `src/world/GameWorld.h/.cpp` |
| 4 | Thêm vào CMakeLists.txt | `CMakeLists.txt` |
| 5 | Thêm `getMapPixelSize()` vào IMapManager | `IMapManager.h`, `MapManager.h/.cpp` |
| 6 | Build, test, debug | — |

