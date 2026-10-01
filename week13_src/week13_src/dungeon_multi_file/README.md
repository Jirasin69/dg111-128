# Week 13 — Multi-File Raylib Example (Dungeon Explorer)

ตัวอย่างประกอบ [slide_spec_week13.md](../../slide_spec_week13.md) — เกมเดียวกับ [Week 12 dungeon_multi_file](../../../week_12/src/dungeon_multi_file/README.md) แต่เปลี่ยน render + input เป็น Raylib

โค้ดชุดเดียวกับ [../dungeon_single_file/](../dungeon_single_file/) แต่แยกเป็น 4 ส่วนตามหน้าที่ (`.h` + `.c`) — จุดสำคัญคือ **แก้แค่ `render.c` กับ `main.c`** ส่วน `game.h/c` และ `save.h/c` คัดลอกมาจาก Week 12 โดยไม่แก้แม้แต่บรรทัดเดียว นี่คือผลตอบแทนของการแยก logic ออกจาก render ตั้งแต่ Week 12

## 🧱 โครงสร้างไฟล์

| ไฟล์ | หน้าที่ | เปลี่ยนจาก Week 12 หรือไม่ |
|------|---------|------------------------------|
| game.h | struct (Player, Enemy, GameState) + header guard + prototype รวม | ไม่เปลี่ยน |
| game.c | tilemap, เดิน, ต่อสู้อัตโนมัติเมื่อชน enemy, ลงชั้นถัดไป | ไม่เปลี่ยน |
| render.h | prototype เดิมทุกตัว + `TILE`, `SCREEN_W`, `SCREEN_H` (ขนาดหน้าต่าง) | **เพิ่ม** `#define` ขนาดหน้าต่าง |
| render.c | วาดจอด้วย `DrawRectangle`/`DrawText` + `getPlayerInput()` ด้วย `IsKeyPressed()` | **เปลี่ยน** (Raylib แทน printf/`system("cls")`/`getch()`) |
| save.h / save.c | `fwrite`/`fread` บันทึก GameState ทั้งก้อน | ไม่เปลี่ยน |
| main.c | `InitWindow` + real-time loop 60 FPS: input → update → save เมื่อสั่ง → draw ทุกเฟรม | **เปลี่ยน** (turn-based → real-time loop) |

## ▶️ Compile & Run

```bash
gcc -std=c99 -Wall main.c game.c render.c save.c -o game -lraylib -lopengl32 -lgdi32 -lwinmm
./game
```

## 🎮 วิธีเล่น

พิมพ์ชื่อผู้เล่นในหน้าต่าง console ก่อน (กด Enter) จากนั้นหน้าต่าง Raylib จะเปิดขึ้น

| ปุ่ม | การกระทำ |
|------|---------|
| W/A/S/D หรือปุ่มลูกศร | เดิน (ชนสี่เหลี่ยมสีแดง = ต่อสู้อัตโนมัติ, ถึงช่องสีทอง = ลงชั้นถัดไป) |
| V | บันทึกเกม (save) |
| Q | ออก (ไปหน้า Game Over) |
| ESC / ปิดหน้าต่าง | ออกจากโปรแกรมทันที |

โปรแกรมบันทึก `dungeon_save.bin` เฉพาะตอนผู้เล่นกด V เท่านั้น (ไม่ auto-save)

> ไม่ต้องใช้ `conio.h` อีกแล้ว — Raylib ทำงานได้ทั้ง Windows/Linux/Mac (เปลี่ยนแค่ flag ตอน link)

## 🔍 เทียบกับ Week 12

```bash
# ไม่มีผลลัพธ์ = ไฟล์เหมือนกันทุกบรรทัด
diff ../../../week_12/src/dungeon_multi_file/game.c game.c
diff ../../../week_12/src/dungeon_multi_file/save.c save.c
```

| Week 12 (render.c) | Week 13 (render.c) |
|--------------------|--------------------|
| `system("cls")` | `ClearBackground(BLACK)` ภายใน `BeginDrawing()`/`EndDrawing()` |
| `printf("#")`, `printf("@")` | `DrawRectangle(...)` สีต่างกันตาม tile |
| `printBar()` แบบ `[####    ]` | `drawHpBar()` ด้วย `DrawRectangle` |
| `getch()` (รอจนกว่าจะกด) | `IsKeyPressed()` (เช็คแล้วคืน 0 ทันทีถ้าไม่ได้กด) |

## 💡 ให้ลองทำ

- ลอง `#include "raylib.h"` ใน game.c แล้วถามตัวเองว่า "game.c ต้องรู้จัก Raylib ไหม?" — ถ้าไม่ต้อง ก็ไม่ควร include
- เปลี่ยน `TILE` ใน render.h จาก 48 เป็น 32 — หน้าต่างและทุก tile ย่อตามโดยไม่ต้องแก้ game.c
- แก้ `drawMap()` ให้วาดผู้เล่นเป็นวงกลมด้วย `DrawCircle()` แทนสี่เหลี่ยม
- ลบ `ClearBackground(BLACK)` ออกแล้วดูว่าเกิดอะไรขึ้น (เทียบกับการลบ `system("cls")` ใน Week 12)
- ใช้ `loadGame()` ที่มีอยู่แล้วใน save.c: เพิ่มปุ่ม `L` ใน `getPlayerInput()` แล้วเรียก `loadGame()` ใน main.c
