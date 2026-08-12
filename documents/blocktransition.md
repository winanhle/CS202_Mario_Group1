# Kế hoạch Chuyển Trạng Thái Block (Block Transition)

*Xử lý logic chuyển trạng thái + đổi texture cho tile khi player đập từ dưới lên (`onHitFromBelow`)*

---

## 1. Yêu cầu (Transition Table)

| Block ban đầu (`TileType`) | Sau khi hit từ dưới | `m_mapData` (logic) | `m_rawGids` (texture render) |
|---|---|---|---|
| `HIDDEN_BLOCK` | → `SOLID_BRICK` | `SOLID_BRICK` | GID của `SOLID_BRICK` (hiện tại **2667**) |
| `BRICK_NORMAL` | → `EMPTY` | `EMPTY` | **0** (không có tile → không vẽ) |
| `QUESTION_COIN` | → `SOLID_BRICK` | `SOLID_BRICK` | GID của `SOLID_BRICK` (**2667**) |
| `QUESTION_POWERUP` | → `SOLID_BRICK` | `SOLID_BRICK` | GID của `SOLID_BRICK` (**2667**) |
| `MULTI_COIN` | → `SOLID_BRICK` | `SOLID_BRICK` | GID của `SOLID_BRICK` (**2667**) |

> **Quy ước "load texture X"** trong project này = gán lại GID trong `m_rawGids`.
> Renderer vẽ texture bằng cách crop UV rect từ sprite-sheet theo GID (MapManager.cpp:377-399).
> GID = `0` ⇔ "không có tile" ⇔ texture trống (EMPTY). GID khác 0 ⇔ vẽ đúng ô tile trong tileset.

---

## 2. Phân tích vấn đề hiện tại (Root Cause)

Hiện có **2 grid song song** trong `MapManager`:

- `m_mapData` — lưu `TileType` (phục vụ collision + logic trò chơi).
- `m_rawGids` — lưu GID gốc từ Tiled (phục vụ render sprite-sheet).

Các handler hiện tại **chỉ sửa `m_mapData` mà không sửa `m_rawGids`**, dẫn đến:
texture trên màn hình không bao giờ đổi dù loại block đã đổi.

Cụ thể (MapManager.cpp):

| Handler | Dòng | Chỉ sửa `m_mapData` | Bug texture |
|---|---|---|---|
| `handleBrickNormal` | 538-543 | → `EMPTY` | Vẫn vẽ GID brick cũ (1920) → gạch không vỡ |
| `handleQuestionCoin` | 545-554 | → `SOLID_BRICK` | Vẫn vẽ GID ?-block cũ (1916) → không thành block đã dùng |
| `handleQuestionPowerup` | 556-572 | → `SOLID_BRICK` | Vẫn vẽ GID ?-block cũ (1921/1078) |
| `handleMultiCoin` | 574-590 + update 302-319 | → `SOLID_BRICK` (sau 3.5s) | Vẫn vẽ GID multi-coin cũ (1923) |
| `handleHiddenBlock` | 592-600 | giữ `HIDDEN_BLOCK` + set `m_revealedHiddenBlocks` | Dùng cơ chế riêng; sau hit vẫn hiện texture hidden-block |

Ngoài ra còn có vấn đề thiết kế:
- `HIDDEN_BLOCK` dùng một cơ chế riêng `m_revealedHiddenBlocks` (MapManager.h:83) thay vì chuyển trạng thái thật sự → code render (363-372) và `isSolid` (482-486) có special-case.
- `MULTI_COIN` chuyển trạng thái bên trong `update()` (302-319) thay vì trong handler → tách rời logic.

---

## 3. Thiết kế OOP

### 3.1 Nguyên tắc OOP áp dụng

1. **Encapsulation (Đóng gói)** — Mọi thay đổi tile phải đi qua **một** method duy nhất `setTile(gx, gy, type)` để đảm bảo `m_mapData` và `m_rawGids` luôn đồng bộ. Không cho phép sửa trực tiếp 2 grid ở nơi khác.
2. **Single Responsibility (SRP)** — Tách "hành vi khi bị đập" ra khỏi `MapManager` thành các class `BlockBehavior`. `MapManager` chỉ còn làm nhiệm vụ điều phối (dispatch).
3. **Open/Closed (OCP)** — Muốn thêm loại block mới = thêm 1 subclass `BlockBehavior`, **không sửa** `MapManager` (không thêm case vào switch).
4. **Strategy/State Pattern** — Giống hệt mô hình `IPlayerForm` đã dùng cho player (xem `documents/player_system_design_plan.md`). `MapManager` giữ `std::unique_ptr` / map type→behavior và ủy quyền.
5. **Dependency Inversion (DIP)** — `MapManager` phụ thuộc interface `IBlockBehavior`, không phụ thuộc concrete class.

### 3.2 Sơ đồ lớp

```text
┌─────────────────────────────────────────────┐
│              MapManager                       │  (Điều phối – chỉ dispatch)
│  + setTile(gx, gy, TileType)                 │  ← sửa CẢ 2 grid, là nơi duy nhất
│  + onHitFromBelow(...)                       │
│  - unordered_map<TileType, int> m_typeToGid  │  ← "texture" của từng loại
│  - unordered_map<TileType, IBlockBehavior*>  │  ← hành vi của từng loại
└──────────────────────┬───────────────────────┘
                       │ delegates (Strategy)
┌──────────────────────┴───────────────────────┐
│                IBlockBehavior (abstract)      │
│  + bool   isSolid() const                    │
│  + int    getRenderGid() const               │
│  + void   onHitFromBelow(MapManager&, gx, gy,│
│                          formType)           │
└──────────────────────┬───────────────────────┘
                       │ implements
  ┌──────────────┬──────┴─────┬───────────────┬──────────────┐
  │              │            │               │              │
GroundBehavior  PipeBehavior  BrickBehavior QuestionCoin  QuestionPowerup
                               │               Behavior      Behavior
                               │ (onHit: debris │(onHit: coin    │ (onHit: item
                               │  + setTile EMPTY)│pop + SOLID_BRICK)│  + SOLID_BRICK)
  ┌──────────────┬─────────────┼──────────────┬─────────────┐
  │              │             │              │             │
MultiCoinBehavior HiddenBlock SolidBrickBehavior EmptyBehavior ...
                 Behavior
```

### 3.3 Ví dụ: `BrickBehavior` (thu gọn)

```cpp
// MapManager trở thành "collaborator" để behavior gọi side-effect
void BrickBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) {
    map.spawnBrickDebris(gx, gy);   // hiệu ứng mảnh gạch (side-effect cũ giữ nguyên)
    map.setTile(gx, gy, TileType::EMPTY);  // → EMPTY + GID = 0 (biến mất)
}

void QuestionPowerupBehavior::onHitFromBelow(MapManager& map, int gx, int gy, int formType) {
    map.spawnItemForFormType(gx, gy, formType);   // Mushroom / FireFlower
    map.setTile(gx, gy, TileType::SOLID_BRICK);   // → block đã dùng + đổi texture
}
```

---

## 4. Các bước triển khai chi tiết

> Nên làm **2 phase**. Phase 1 bắt buộc (sửa bug đồng bộ texture).
> Phase 2 là refactor OOP để đạt yêu cầu thiết kế. Có thể merge sau.

### Phase 1 — Sửa bug texture (bắt buộc, nhanh)

**Bước 1: Thêm reverse map `m_typeToGid` và method `setTile` (MapManager.h)**

```cpp
// MapManager.h
private:
    std::unordered_map<TileType, int> m_typeToGid; // type → GID render (EMPTY → 0)
    ...
    void setTile(int gx, int gy, TileType type);   // sửa CẢ m_mapData + m_rawGids
```

**Bước 2: Build `m_typeToGid` trong `loadTileset()` (MapManager.cpp:92-186)**

Sau vòng lặp đọc `<tile>` (dòng 177-180), thêm:
```cpp
// Đảo ngược m_gidTypeMap. Lưu ý: nếu nhiều GID cùng 1 type, GID nào cũng vẽ
// đúng sprite; EMPTY luôn ánh xạ về 0 (không có tile → không vẽ).
m_typeToGid[TileType::EMPTY] = 0;
for (auto& [gid, type] : m_gidTypeMap)
    m_typeToGid[type] = gid;
```

**Bước 3: Implement `setTile` (MapManager.cpp)**

```cpp
void MapManager::setTile(int gx, int gy, TileType type) {
    if (gy < 0 || gy >= (int)m_mapData.size() ||
        gx < 0 || gx >= (int)m_mapData[0].size()) return;
    m_mapData[gy][gx] = type;
    auto it = m_typeToGid.find(type);
    m_rawGids[gy][gx] = (it != m_typeToGid.end()) ? it->second : 0;
}
```

**Bước 4: Sửa 5 handler để gọi `setTile` (MapManager.cpp)**

| Handler | Sửa thành |
|---|---|
| `handleBrickNormal` (538) | giữ `spawnBrickDebris`; `setTile(gx, gy, EMPTY)` |
| `handleQuestionCoin` (545) | giữ coin-pop + `spawnCoinPop`; `setTile(gx, gy, SOLID_BRICK)` |
| `handleQuestionPowerup` (556) | giữ spawn item; `setTile(gx, gy, SOLID_BRICK)` |
| `handleMultiCoin` (574) | giữ coin-pop; countdown khi hết → `setTile(gx, gy, SOLID_BRICK)` (sửa dòng 312) |
| `handleHiddenBlock` (592) | đổi thành `setTile(gx, gy, SOLID_BRICK)` (bỏ set `m_revealedHiddenBlocks`) |

**Bước 5: Bỏ special-case `HIDDEN_BLOCK` (MapManager.cpp)**

- Render (dòng 363-372): thay khối `if (type == HIDDEN_BLOCK) {...}` bằng `if (type == TileType::HIDDEN_BLOCK) continue;` (luôn ẩn; sau khi hit nó đã là `SOLID_BRICK` nên không còn tồn tại dạng HIDDEN).
- `isSolid` (dòng 482-486): bỏ nhánh `m_revealedHiddenBlocks`, trả `false` cho `HIDDEN_BLOCK`.
- Xóa member `m_revealedHiddenBlocks` trong MapManager.h:83.
- Fallback color (dòng 408-409): bỏ `HIDDEN_BLOCK` khỏi cùng nhóm màu với `BRICK_NORMAL` (không còn cần).

### Phase 2 — Refactor theo State/Strategy Pattern (OOP)

**Bước 6: Tạo interface `IBlockBehavior`** — `src/entities/map/block/IBlockBehavior.h`

```cpp
class MapManager;

class IBlockBehavior {
public:
    virtual ~IBlockBehavior() = default;
    virtual bool isSolid() const = 0;
    virtual int  getRenderGid() const = 0;
    virtual void onHitFromBelow(MapManager& map, int gx, int gy, int formType) = 0;
};
```

**Bước 7: Tạo các behavior class**
`GroundBehavior, PipeBehavior, BrickBehavior, QuestionCoinBehavior,
QuestionPowerupBehavior, MultiCoinBehavior, HiddenBlockBehavior,
SolidBrickBehavior, EmptyBehavior, DeathZoneBehavior, FlagpoleBehavior, CoinBehavior`.

Mỗi class implement 3 phương thức theo transition table ở mục 1.

**Bước 8: `MapManager` dùng behavior**

- Thay `switch (type)` trong `onHitFromBelow` (507-532) bằng lookup:
  `if (auto* b = behaviorOf(type)) b->onHitFromBelow(*this, gx, gy, formType);`
- `behaviorOf` có thể là factory nhỏ (static map `TileType → IBlockBehavior*`).
- `isSolid` ủy quyền: `return behaviorOf(type)->isSolid();`
- Renderer ủy quyền lấy GID: ưu tiên `m_rawGids` (giữ nguyên); fallback color map theo type.

**Bước 9: Di chuyển side-effect vào public API của MapManager**
Các method hiện đang `private` mà behavior cần gọi (spawn debris, coin pop, spawn item)
phải thành `public` hoặc `friend class IBlockBehavior`. Khuyến nghị `public`:
`spawnBrickDebris(int,int)`, `spawnCoinPopAt(int,int)`, `spawnItemForFormType(int,int,int)`.

---

## 5. Chỉnh tileset & map trong Tiled

### 5.1 Tileset (`assets/tileset/tileset_test.tsx`)

Điều kiện cần đủ để transition texture hoạt động:

1. **Phải có đúng 1 tile đại diện cho block "đã dùng / exhausted"** mang property `type = SOLID_BRICK`.
   - Hiện tại đã có: `tile id=2666` → GID **2667** (MapManager gọi nó là `SOLID_BRICK`).
   - Texture của tile này nên vẽ hình block nhạt màu (đã "mở") khác hẳn `BRICK_NORMAL` và `?`-block.
2. Mỗi loại block tương tác phải có property `type` đúng:
   - `BRICK_NORMAL` (id 1919), `QUESTION_COIN` (id 1915), `QUESTION_POWERUP` (id 1077, 1920), `MULTI_COIN` (id 1922), `HIDDEN_BLOCK` (id 1).
   - Loader đọc theo thứ tự: `propertytype` → `name` → property `type` (MapManager.cpp:141-173). Project hiện dùng `<property name="type" value="..."/>` → **giữ nguyên**.
3. **Không** cần thêm tile mới nếu đã có đủ các GID trên. Chỉ cần chắc chắn sprite của `SOLID_BRICK` (2667) trông đúng ý.

### 5.2 Cách set property trong Tiled (khi cần sửa)

1. Mở tileset: **Map → Tilesets → Edit Tileset** (hoặc mở thẳng `tileset_test.tsx`).
2. Chọn 1 tile trong sprite-sheet.
3. Cửa sổ **Properties** (bên phải) → **+ Add Property**.
4. Đặt tên `type`, kiểu `string`, value = tên enum (`BRICK_NORMAL`, `SOLID_BRICK`, ...).
5. (Tùy chọn, gọn hơn) Tạo **Class/Enum** trong **Project → Project Properties → Custom Types** tên `TileType` liệt kê các value, rồi gán property với type `TileType` → Tiled sẽ hiện dropdown.

> ⚠️ **Lưu ý quan trọng:** Property phải khớp **chính xác** chuỗi trong `stringToTileType()` (MapManager.cpp:52-64), viết HOA, underscore giữa từ. Ví dụ `QUESTION_POWERUP` chứ không phải `question powerup`.

### 5.3 Map (`assets/map/map_test.tmx`)

- Tile được đặt trên layer bằng GID thô; game map GID → `TileType` qua `m_gidTypeMap`.
- **Không cần đổi nội dung layer** — chỉ cần đảm bảo:
  1. Layer đầu tiên (layer id=1) chứa toàn bộ tile logic/collision. Các layer sau chỉ là trang trí (hiện game chỉ đọc layer đầu, MapManager.cpp:230-234).
  2. Data encoding giữ nguyên `csv` (game chỉ hỗ trợ csv, MapManager.cpp:239-243).
  3. `firstgid` trong thẻ `<tileset>` không đổi (=1), nếu không mọi GID sẽ lệch.
- Sau khi chỉnh trong Tiled nhớ **Ctrl+S** — file `.tsx`/`.tmx` được game đọc trực tiếp lúc chạy (không cần import/build lại asset).

### 5.4 Checklist Tiled

- [ ] Tile `SOLID_BRICK` (id 2666) có texture "block đã dùng" rõ ràng
- [ ] 5 loại block tương tác đều có property `type` đúng chuỗi
- [ ] `firstgid = 1`, encoding = csv
- [ ] Save cả `.tsx` và `.tmx`

---

## 6. Kiểm thử (Test Plan)

Sau khi implement, chạy game và test từng trường hợp:

| Thao tác | Kỳ vọng |
|---|---|
| Đập `HIDDEN_BLOCK` từ dưới | Trước khi đập: vô hình + không chặn di chuyển. Sau khi đập: trở thành block solid có texture `SOLID_BRICK` |
| Đập `BRICK_NORMAL` từ dưới | Mảnh gạch bay + tile **biến mất** (không còn texture, không chặn) |
| Đập `QUESTION_COIN` | Coin pop + nhận coin + block hóa thành `SOLID_BRICK` (texture đổi ngay) |
| Đập `QUESTION_POWERUP` | Spawn Mushroom/FireFlower + block hóa `SOLID_BRICK` |
| Đập `MULTI_COIN` | Coin pop mỗi lần đập trong cửa sổ 3.5s; hết cửa sổ → `SOLID_BRICK` |
| `EMPTY` / tile trang trí | Không vẽ, không chặn, đập không phản ứng |

> **Quyết định mở (open decision):** `MULTI_COIN` — chuyển `SOLID_BRICK` **ngay lần đập đầu** (đúng nghĩa đen yêu cầu) hay **sau 3.5s** (giữ gameplay multi-coin như Mario gốc)? Khuyến nghị **giữ 3.5s** vì đó là bản chất của multi-coin; chỉ cần đảm bảo khi hết giờ texture cũng đổi nhờ `setTile`.

---

## 7. Tóm tắt nguyên lý OOP được bảo đảm

| Nguyên lý | Áp dụng ở đâu |
|---|---|
| Encapsulation | `setTile()` là điểm duy nhất sửa tile → 2 grid không bao giờ lệch |
| SRP | Hành vi từng block nằm trong từng `IBlockBehavior` subclass, không còn switch dài |
| OCP | Thêm block mới = thêm subclass + đăng ký, không sửa `MapManager` |
| Strategy | `MapManager` ủy quyền cho `IBlockBehavior` (tương tự `IPlayerForm`) |
| DIP | `MapManager` phụ thuộc interface, không phụ thuộc concrete |

---

## 8. File thay đổi (tổng kết)

- `src/entities/map/MapManager.h` — bỏ `m_revealedHiddenBlocks`; thêm `m_typeToGid`, `setTile`, map behaviors
- `src/entities/map/MapManager.cpp` — build `m_typeToGid`; sửa 5 handler; sửa render/isSolid/update; bỏ special-case HIDDEN
- `src/entities/map/block/IBlockBehavior.h` — **(mới)** interface
- `src/entities/map/block/*Behavior.h/.cpp` — **(mới)** các behavior concrete
- `assets/tileset/tileset_test.tsx` — (chỉ khi cần) chỉnh sprite `SOLID_BRICK`
- `assets/map/map_test.tmx` — thường **không cần đổi**
