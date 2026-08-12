# Class Design – Fire Shooting Mechanic

## Tổng quan kiến trúc

```
FireForm (khi Mario nhấn Shoot)
    │  shoot()  →  tạo Fireball
    ▼
FireballManager  ─── owns ──►  std::vector<Fireball>   (value, không dùng pointer)
    │                               │
    │  update()                     │  State Machine
    │                               ▼
    │                          Fireball
    │                     (Flying → Exploding → Dead)
    │
    ├── collision với Enemy  →  enemy.takeDamageFromFireball()
    └── collision với Map    →  bounce / explode
```

> [!NOTE]
> Không có `IFireball` interface vì không có kế hoạch thêm loại đạn khác.
> `FireballManager` quản lý trực tiếp `std::vector<Fireball>` (by value),
> tránh heap allocation thừa và giảm indirection.

---

## 1. FireballState – Enum trạng thái cầu lửa

```
src/entities/projectile/FireballState.h
```

```cpp
enum class FireballState {
    Flying,    // đang bay, quay 4 frame
    Exploding, // nổ, phát 3 frame explosion
    Dead       // xong, FireballManager xóa
};
```

> [!NOTE]
> Không có state `Bouncing` riêng — bounce được xử lý bên trong `Flying`
> thông qua counter `m_bounceCount`. State chỉ chuyển sang `Exploding`
> khi hết bounce hoặc va chạm trục X.

---

## 2. `Fireball` – Concrete entity (State Machine)

```
src/entities/projectile/Fireball.h
src/entities/projectile/Fireball.cpp
```

### Members

```cpp
class Fireball {
    // ── Physics ──────────────────────────────────────────────
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;       // vx = SPEED * direction, vy = 0 ban đầu
    static constexpr float SPEED        = 250.f;
    static constexpr float GRAVITY      = 600.f;
    static constexpr float BOUNCE_VY    = -200.f; // vận tốc nảy lên sau va chạm Y
    static constexpr int   MAX_BOUNCES  = 2;

    // ── State ─────────────────────────────────────────────────
    FireballState m_state = FireballState::Flying;
    int           m_bounceCount = 0;

    // ── Animation ─────────────────────────────────────────────
    // Texture được giữ bởi FireballManager, Fireball chỉ giữ tham chiếu
    const sf::Texture* m_texture = nullptr;
    sf::Sprite         m_sprite;
    int                m_currentFrame = 0;
    float              m_animTimer    = 0.f;
    static constexpr float FLY_FRAME_TIME     = 0.08f; // 4 frame fly
    static constexpr float EXPLODE_FRAME_TIME = 0.07f; // 3 frame explode

public:
    // Constructor nhận texture từ FireballManager
    Fireball(float x, float y, int direction, const sf::Texture& texture);

    void update(float dt, const IMapManager& map, IEnemyManager& enemies);
    void render(sf::RenderWindow& window) const;

    bool isDead()             const { return m_state == FireballState::Dead; }
    sf::FloatRect getHitbox() const;

private:
    void applyGravity(float dt);
    void handleTileCollision(const IMapManager& map, float dt);
    void handleEnemyCollision(IEnemyManager& enemies);
    void updateFlyAnimation(float dt);
    void updateExplodeAnimation(float dt);
};
```

### State Machine

```
Flying ──[va chạm Y + bounceCount < 2]──► Flying  (bounce: vy = -200)
Flying ──[va chạm Y + bounceCount == 2]──► Exploding
Flying ──[va chạm X]────────────────────► Exploding
Flying ──[ra khỏi map / lifetime > 4s]──► Dead (không qua Exploding)

Exploding ──[animation xong (3 frames)]──► Dead
```

### Methods chính

```cpp
void Fireball::update(float dt, const IMapManager& map, IEnemyManager& enemies) {
    switch (m_state) {
    case FireballState::Flying:
        applyGravity(dt);
        handleTileCollision(map, dt);  // → xác định bounce hay explode
        handleEnemyCollision(enemies); // → nếu trúng enemy → explode
        updateFlyAnimation(dt);        // xoay 4 frame
        break;

    case FireballState::Exploding:
        updateExplodeAnimation(dt);    // 3 frame, rồi → Dead
        break;

    case FireballState::Dead:
        break;
    }
}
```

### Tile Collision (tách trục X và Y)

```cpp
void Fireball::handleTileCollision(const IMapManager& map, float dt) {
    // ── Trục Y (kiểm tra trước) ──
    float newY = m_position.y + m_velocity.y * dt;
    if (collidesWithMap(map, m_position.x, newY)) {
        if (m_bounceCount < MAX_BOUNCES) {
            m_velocity.y = BOUNCE_VY;
            ++m_bounceCount;
            // KHÔNG chuyển state, vẫn Flying
        } else {
            m_state = FireballState::Exploding; // hết bounce → nổ
        }
    } else {
        m_position.y = newY;
    }

    // ── Trục X ──
    float newX = m_position.x + m_velocity.x * dt;
    if (collidesWithMap(map, newX, m_position.y)) {
        m_state = FireballState::Exploding; // va tường → nổ ngay
    } else {
        m_position.x = newX;
    }
}
```

---

## 3. `FireballManager` – Quản lý vòng đời cầu lửa

```
src/entities/projectile/FireballManager.h
src/entities/projectile/FireballManager.cpp
```

`FireballManager` owns texture và quản lý trực tiếp `std::vector<Fireball>`.
Mỗi `Fireball` được lưu **by value** — không cần heap allocation riêng,
cache-friendly và đơn giản hơn.

```cpp
class FireballManager {
    std::vector<Fireball> m_fireballs;        // by value, không dùng pointer
    sf::Texture           m_fireballTexture;  // owns texture, truyền ref cho Fireball
    IMapManager*          m_mapManager   = nullptr;
    IEnemyManager*        m_enemyManager = nullptr;

public:
    void initialize();   // load texture từ FireballSprite::Sheet
    void update(float dt);
    void render(sf::RenderWindow& window) const;

    // Gọi từ PlayerManager::shoot()
    void spawnFireball(float x, float y, int direction /*-1 hoặc +1*/);

    void setMapManager(IMapManager* map)       { m_mapManager   = map; }
    void setEnemyManager(IEnemyManager* enemies){ m_enemyManager = enemies; }

    int getActiveCount() const { return static_cast<int>(m_fireballs.size()); }
};
```

**`update()`** dọn dẹp cầu lửa đã `Dead`:
```cpp
void FireballManager::update(float dt) {
    for (auto& fb : m_fireballs)
        fb.update(dt, *m_mapManager, *m_enemyManager);

    // Xóa các cầu lửa đã chết (Erase-Remove Idiom)
    std::erase_if(m_fireballs, [](const Fireball& fb){ return fb.isDead(); });
}

void FireballManager::spawnFireball(float x, float y, int direction) {
    m_fireballs.emplace_back(x, y, direction, m_fireballTexture);
}
```

> [!NOTE]
> Dùng `emplace_back` để construct `Fireball` in-place trong vector,
> tránh copy thừa. `Fireball` cần có move constructor (compiler tự sinh).
> Vì texture được truyền bằng `const&`, `Fireball` chỉ giữ pointer tới
> texture của manager — không bao giờ dùng sau khi manager bị hủy.

---

## 4. Shoot Cooldown – trong `FireForm`

```
src/entities/player/forms/FireForm.h  (MODIFY)
```

```cpp
class FireForm : public IPlayerForm {
    // ... (giữ nguyên hiện tại)

    // ── Shoot Cooldown ────────────────────────────────────────
    float m_shootCooldown    = 0.f;
    static constexpr float SHOOT_COOLDOWN_MAX = 0.5f; // giây

public:
    // Gọi mỗi frame để tick down cooldown
    void updateCooldown(float dt);

    // Trả về true nếu có thể bắn
    bool canShoot() const { return m_shootCooldown <= 0.f; }

    // Gọi sau khi bắn để reset cooldown
    void triggerShootCooldown() { m_shootCooldown = SHOOT_COOLDOWN_MAX; }
};
```

> [!NOTE]
> Cooldown nằm trong `FireForm` chứ không phải `PlayerManager` để đảm bảo
> **SRP**: khi demote về `SuperForm`, cooldown biến mất cùng form.

---

## 5. `ShootCommand` – Command Pattern (mở rộng)

```
src/entities/player/input/Command.h  (MODIFY – thêm class)
```

```cpp
class ShootCommand : public Command {
public:
    void execute(PlayerManager& player) override;
    // gọi player.shoot()
};
```

```
src/entities/player/PlayerManager.h  (MODIFY – thêm method)
```

```cpp
void shoot(); // kiểm tra form hiện tại là FireForm không, gọi FireballManager
```

```
src/entities/player/input/PlayerInputHandler.h  (MODIFY – thêm binding)
```

```cpp
struct KeyBinding {
    // ... giữ nguyên 7 phím cũ ...
    sf::Keyboard::Key shoot; // phím bắn, ví dụ LShift / F
};
```

---

## 6. `shoot()` – Logic bắn trong `PlayerManager`

```cpp
void PlayerManager::shoot() {
    // Chỉ bắn nếu đang là FireForm VÀ cooldown xong
    if (auto* ff = dynamic_cast<FireForm*>(m_currentForm.get())) {
        if (ff->canShoot() && m_fireballManager) {
            int dir = (m_velocityX >= 0.f) ? 1 : -1; // theo hướng đang nhìn
            m_fireballManager->spawnFireball(m_positionX, m_positionY, dir);
            ff->triggerShootCooldown();
        }
    }
}
```

> [!IMPORTANT]
> Dùng `dynamic_cast` để kiểm tra form là hợp lý ở đây vì shoot là hành vi
> đặc thù của `FireForm`. Nếu muốn tránh hoàn toàn, thêm:
> ```cpp
> virtual bool canShoot() const { return false; } // IPlayerForm default
> virtual void shoot() {}                          // IPlayerForm default
> ```
> rồi `FireForm` override cả hai.

---

## 7. Kết nối dependency – `GameWorld::injectDependencies()`

```cpp
void GameWorld::injectDependencies() {
    // ... (giữ nguyên cũ)

    // FireballManager ← EnemyManager (để fireball có thể hit enemy)
    if (m_playerManager)
        m_playerManager->setFireballEnemyTarget(m_enemyManager.get());

    if (m_playerManager2)
        m_playerManager2->setFireballEnemyTarget(m_enemyManager.get());
}
```

`PlayerManager` sở hữu `FireballManager`:
```
PlayerManager
  └── unique_ptr<FireballManager> m_fireballManager
        ├── IMapManager*   (inject từ setMapManager())
        └── IEnemyManager* (inject từ setFireballEnemyTarget())
```

---

## 8. Animation Sprites

```
src/entities/projectile/FireballSprite.h  (NEW)
```

```cpp
namespace FireballSprite {
    constexpr const char* Sheet = "assets/texture/projectile/fireball.png";
    // Sprite sheet ngang: 4 fly frame (8x8) + 3 explode frame (16x16)

    constexpr sf::IntRect FlyFrames[4] = {
        {{0,  0}, {8, 8}},
        {{8,  0}, {8, 8}},
        {{16, 0}, {8, 8}},
        {{24, 0}, {8, 8}},
    };
    constexpr sf::IntRect ExplodeFrames[3] = {
        {{32, 0}, {16, 16}},
        {{48, 0}, {16, 16}},
        {{64, 0}, {16, 16}},
    };
}
```

> [!NOTE]
> Pixel offsets là placeholder — cần đo từ sprite sheet thực tế sau khi có asset.

---

## 9. Tổng hợp file

| File | Loại | Nội dung |
|------|------|----------|
| `src/entities/projectile/FireballState.h` | **NEW** | Enum trạng thái |
| `src/entities/projectile/Fireball.h/.cpp` | **NEW** | Concrete entity, state machine |
| `src/entities/projectile/FireballManager.h/.cpp` | **NEW** | `vector<Fireball>` + update/render |
| `src/entities/projectile/FireballSprite.h` | **NEW** | Sprite offsets (placeholder) |
| `src/entities/player/forms/FireForm.h/.cpp` | MODIFY | Thêm cooldown |
| `src/entities/player/input/Command.h/.cpp` | MODIFY | Thêm `ShootCommand` |
| `src/entities/player/input/PlayerInputHandler.h/.cpp` | MODIFY | Thêm `shoot` key |
| `src/entities/player/PlayerManager.h/.cpp` | MODIFY | `shoot()`, `m_fireballManager`, `setFireballEnemyTarget()` |
| `src/interfaces/IPlayerManager.h` | MODIFY | `setFireballEnemyTarget()` |
| `src/world/GameWorld.cpp` | MODIFY | Inject EnemyManager vào Fireball |
| `CMakeLists.txt` | MODIFY | Thêm `Fireball.cpp`, `FireballManager.cpp` |

---

## Luật OOP được đảm bảo

| Nguyên tắc | Áp dụng |
|------------|---------|
| **SRP** | `Fireball` tự quản lý state/physics/animation; `FireballManager` chỉ quản lý vòng đời (spawn, update, cleanup); cooldown nằm trong `FireForm` |
| **OCP** | Không dùng interface vì không cần mở rộng; nhưng `FireballManager` vẫn kín với `Fireball` — thêm tính năng mới (max count, particle effect) chỉ cần sửa 1 class |
| **DIP** | `FireballManager` phụ thuộc `IMapManager` & `IEnemyManager` (interface), không phụ thuộc concrete `MapManager` hay `EnemyManager` |
| **Command Pattern** | `ShootCommand` nhất quán với pattern cũ (`JumpCommand`, `MoveLeftCommand`, ...); Mario không bị coupling trực tiếp với phím |
| **State Machine** | `FireballState` enum + `switch` trong `Fireball::update()` — state chuyển rõ ràng, không dùng cờ boolean rời rạc |
| **Ownership rõ ràng** | `FireballManager` owns texture + `vector<Fireball>`; `Fireball` chỉ giữ `const Texture*` (non-owning pointer) |
| **Value semantics** | `vector<Fireball>` by value thay vì `vector<unique_ptr<...>>` — đơn giản hơn, cache-friendly, không cần virtual destructor |
