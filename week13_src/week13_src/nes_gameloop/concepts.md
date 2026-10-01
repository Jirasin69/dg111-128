# องค์ความรู้จาก Course ที่ใช้ใน `main.c`

> NES-style FSM Game Loop ด้วย Raylib — รวบรวมทักษะจาก Week 2 ถึง Week 13

---

## Week 2–3: Preprocessor & Constants

```c
#define NES_W   320
#define NES_H   240
#define SCALE   2.5f
#define WIN_W   ((int)(NES_W * SCALE))   /* macro expression */
#define MAX_BULLETS  16
```

- `#define` สำหรับค่าคงที่ — หลีกเลี่ยง magic number
- Macro expression เช่น `WIN_W = NES_W * SCALE` (320 × 2.5 = 800) แสดง composition ของค่าคงที่
- `CLITERAL(Color){r,g,b,a}` — compound literal ของ Raylib สำหรับกำหนดสี palette

---

## Week 3–4: Control Flow

```c
switch (state) {
    case STATE_TITLE:   ...  break;
    case STATE_PLAYING: ...  break;
    case STATE_GAME_OVER: ... break;
}
```

- `switch/case/break` — เลือก logic ตาม state
- `for` loop วนซ้ำ array: bullets, enemies
- **Nested for loop** — ตรวจ bullet vs enemy collision ทุก pair (O(n²))
- `if/else` + compound condition: `dx > -SPR && dx < SPR`

---

## Week 5: Functions

```c
static void TileText(const char *text, int col, int row, Color c) { ... }
static void DrawSpr(float x, float y, Color c) { ... }
```

- นิยาม function ด้วย parameter และ return type
- `static` function — scope จำกัดในไฟล์เดียว (internal linkage)
- `const char *` — pointer to read-only string
- แยก reusable drawing logic ออกจาก `main()`

---

## Week 6: While Loop + Raylib เปิดตัว

```c
while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    // update ...
    BeginDrawing();
        ClearBackground(C_BG);
        // draw ...
    EndDrawing();
}
```

- **Game Loop pattern** — `while (!WindowShouldClose())` เทียบกับ `ppu_wait_nmi()` ของ NES
- `SetTargetFPS(60)` — ล็อก frame rate
- `GetFrameTime()` — delta time (`dt`) สำหรับ frame-rate independent movement
- **Delta time movement**: `playerX += playerSpeed * dt` (ไม่ใช้ `+= speed` คงที่)
- Raylib API พื้นฐาน: `InitWindow`, `CloseWindow`, `BeginDrawing`, `EndDrawing`, `ClearBackground`

---

## Week 7–8: Input Handling

```c
if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    playerX -= playerSpeed * dt;

if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    state = STATE_PLAYING;
```

- `IsKeyDown()` — กดค้างต่อเนื่อง (movement)
- `IsKeyPressed()` — กดครั้งเดียว (confirm/select)
- รองรับ 2 control scheme พร้อมกัน (Arrow + WASD)

---

## Week 8–9: Struct & Arrays

```c
typedef struct {
    float x, y;
    float vx, vy;
    int   active;
} Bullet;

Bullet bullets[MAX_BULLETS] = {0};
Enemy  enemies[MAX_ENEMIES] = {0};
```

- `typedef struct` — นิยาม custom data type สำหรับ entity
- **Static array of structs** — จัดกลุ่ม entity ทั้งหมดไว้ใน array
- `{0}` — zero-initialize array ทั้งหมดในครั้งเดียว
- `memset(bullets, 0, sizeof(bullets))` — reset array เมื่อ restart game
- Dot notation: `bullets[i].x`, `enemies[i].active`

---

## Week 9 & 12: Object Pool Pattern

```c
/* หา slot ว่าง */
for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
        bullets[i].x = playerX + 2;
        bullets[i].active = 1;
        break;
    }
}

/* ข้าม inactive object */
for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) continue;
    bullets[i].y += bullets[i].vy * dt;
}
```

- **Object Pool** — ใช้ static array + `int active` flag แทน malloc ทุก frame
- ค้นหา slot ว่างแล้ว activate, deactivate เมื่อไม่ใช้
- `continue` เพื่อข้าม inactive object — ประหยัด CPU

---

## Week 10–11: Random & Timer

```c
enemies[i].x = (float)GetRandomValue(8, NES_W - 16);

enemyTimer += dt;
if (enemyTimer > 1.0f) {
    enemyTimer = 0.0f;
    /* spawn enemy */
}
```

- `GetRandomValue(min, max)` — เทียบกับ `rand8()` ของ NES
- **Accumulator timer pattern** — บวก `dt` ทุก frame, reset เมื่อถึง threshold
- Cooldown สำหรับ spawn และ shoot แยกกัน (`enemyTimer`, `shootTimer`)

---

## Week 12–13: Finite State Machine (FSM)

```c
typedef enum {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

GameState state = STATE_TITLE;
```

**State transitions:**

```
STATE_TITLE ──[SPACE/ENTER]──▶ STATE_PLAYING
STATE_PLAYING ──[lives <= 0]──▶ STATE_GAME_OVER
STATE_GAME_OVER ──[SPACE/ENTER]──▶ STATE_TITLE
```

- `typedef enum` กำหนด state ที่ชัดเจน — ไม่ใช้ magic number
- `switch(state)` แยก update logic และ draw logic ตาม state
- **Dual-switch pattern**: switch รอบแรก = update, switch รอบสอง = draw

---

## Week 12–13: AABB Collision Detection

```c
float dx = bullets[j].x - enemies[i].x;
float dy = bullets[j].y - enemies[i].y;
if (dx > -SPR && dx < SPR && dy > -SPR && dy < SPR) {
    enemies[i].active = 0;
    bullets[j].active = 0;
    score += 10;
}
```

- **Axis-Aligned Bounding Box (AABB)** — เช็คว่า 2 object ทับกันหรือไม่
- ใช้ขนาด tile `SPR` (8px) เป็น hitbox
- ตรวจสอบทุก bullet × enemy pair ใน nested loop

---

## Week 13: Blink Animation (Frame Counter)

```c
int blinkFrame = 0;
// ...
blinkFrame++;
if ((blinkFrame / 30) % 2 == 0)
    TileText("PRESS SPACE/ENTER", 3, 14, C_WHITE);
```

- **Frame counter** — นับ frame ด้วย `int` และ integer division
- `(frame / 30) % 2` — toggle ทุก 30 frame (= 0.5 วินาที ที่ 60fps)
- เทียบกับ NES ที่ใช้ตัวนับ frame เดียวกัน

---

## สรุป: Concept Map

| Week | หัวข้อ | ปรากฏในโค้ด |
|------|--------|-------------|
| 2–3 | Preprocessor / `#define` | Color palette, WIN_W, MAX_BULLETS |
| 3–4 | Control flow / Switch | FSM switch, nested for, AABB condition |
| 5 | Functions | `TileText()`, `DrawSpr()` |
| 6 | While loop + Raylib | Game loop, delta time, Raylib API |
| 7–8 | Input | `IsKeyDown`, `IsKeyPressed` |
| 8–9 | Struct + Array | `Bullet`, `Enemy`, static arrays |
| 9, 12 | Object Pool | `active` flag, pool loop |
| 10–11 | Random + Timer | `GetRandomValue`, accumulator timer |
| 12–13 | FSM | `GameState` enum, state transitions |
| 12–13 | AABB Collision | bullet vs enemy, player vs enemy |
| 13 | Frame animation | blink counter |

---

> โค้ดนี้ทำหน้าที่เป็น **capstone ของ Phase 1–3** — นำทุก concept ตั้งแต่ Week 2 ถึง 13 มารวมในไฟล์เดียว เหมาะใช้เป็นตัวอย่างก่อนเข้า Mini-Game Sprint (Week 13)
