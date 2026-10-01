# 03_gameloop — NES Shooter (FSM Game Loop)

เกมยิงศัตรูที่ตกลงมา มี 3 สถานะ: `TITLE → PLAYING → GAME_OVER → TITLE` ⭐⭐⭐

← กลับไป [README ของโฟลเดอร์ src](../README.md) · สรุปความรู้ที่ใช้รายสัปดาห์: [concepts.md](concepts.md)

## 🎯 สิ่งที่เรียนรู้
- **FSM แบบ dual-switch:** `switch(state)` รอบแรก = update, รอบสอง = draw
- **Object pool:** `Bullet bullets[16]`, `Enemy enemies[8]` ใช้ `int active` แทน malloc — หา slot ว่างตอน spawn และ `continue` ข้าม slot ที่ไม่ active
- **Timer แบบสะสม:** `enemyTimer += dt` แล้วรีเซ็ตเมื่อเกิน 1 วินาที (spawn ศัตรู), `shootTimer` cooldown 0.25 วินาทีต่อนัด
- **ข้อความกะพริบ** ด้วย `(frame / 30) % 2` = สลับทุก 0.5 วินาทีที่ 60 fps

## 📜 กติกา
- มี 3 ชีวิต
- ยิงศัตรูโดน +10 คะแนน
- ศัตรูชนผู้เล่นหรือหลุดขอบล่างจอ = เสียชีวิต 1
- ชีวิตหมด = GAME OVER (เก็บ Hi-Score ไว้ระหว่างรอบ)

## 🔁 เทียบ dotnes → Raylib

| dotnes | Raylib |
|--------|--------|
| `ppu_wait_nmi()` | `SetTargetFPS(60)` + delta time |
| `GameState enum` | `typedef enum { TITLE, PLAYING, GAME_OVER }` |
| `rand8()` | `GetRandomValue(0, 255)` |
| `music_tick()` | `UpdateMusicStream()` ทุก frame |

## 🎮 ควบคุม
ลูกศร เดิน · `Space` / `Z` ยิง · `Enter` / `Space` เริ่มและเล่นใหม่ · `ESC` ออก

## 🛠️ Compile
```bash
gcc -std=c99 main.c -o gameloop -lraylib -lm -lwinmm -lgdi32
```
