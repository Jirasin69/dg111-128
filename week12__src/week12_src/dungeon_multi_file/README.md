# Week 12 — Multi-File Example (Dungeon Explorer)

ตัวอย่างประกอบ [slide_spec_week12.md](../../slide_spec_week12.md) — เวอร์ชันย่อของ Lab 12 (Mini Dungeon)

โค้ดชุดเดียวกับ [../dungeon_single_file/](../dungeon_single_file/) แต่แยกเป็น 4 ไฟล์ตามหน้าที่ (`.h` + `.c`) — เทียบดูได้ว่า multi-file ต่างจาก single-file ตรงไหน

## 🧱 โครงสร้างไฟล์

| ไฟล์ | หน้าที่ | Slide ที่เกี่ยวข้อง |
|------|---------|---------------------|
| game.h | struct (Player, Enemy, GameState) + header guard + prototype รวม | GameState Struct, Multi-File Architecture — Header Guards |
| game.c | tilemap, เดิน, ต่อสู้อัตโนมัติเมื่อชน enemy, ลงชั้นถัดไป | Game Loop Pattern, GameState Struct |
| render.h / render.c | วาดจอ + `system("cls")` + `getPlayerInput()` ด้วย `getch()` (ไม่มี logic เลย) | Render — ฟังก์ชันล้างหน้าจอ, ปรับปรุงวิธีรับ Input |
| save.h / save.c | `fwrite`/`fread` บันทึก GameState ทั้งก้อน | Binary Save/Load |
| main.c | game loop: draw → input → update → save เมื่อสั่ง (กด V) | Game Loop Pattern |

## ▶️ Compile & Run

```bash
gcc -std=c99 -Wall main.c game.c render.c save.c -o game
./game
```

## 🎮 วิธีเล่น

| ปุ่ม | การกระทำ |
|------|---------|
| W/A/S/D หรือปุ่มลูกศร | เดิน — กดปุ๊บขยับปั๊บ ไม่ต้องกด Enter (ชน `E` = ต่อสู้อัตโนมัติ, ถึง `>` = ลงชั้นถัดไป) |
| V | บันทึกเกม (save) |
| Q | ออก |

โปรแกรมบันทึก `dungeon_save.bin` เฉพาะตอนผู้เล่นกด V เท่านั้น (ไม่ auto-save)

> ⚠️ **conio.h เฉพาะ Windows:** `getch()`/`kbhit()` เป็นฟังก์ชันเฉพาะ Windows (เหมือนที่ [Tetris](../../../../../src/04-sample-console-games/Tetris/main.c) ใช้) ไม่ใช่มาตรฐาน C99 — คอมไพล์และรันบน Linux/Mac ไม่ได้ตรงๆ ถ้าต้องการโค้ดที่ portable ให้ใช้ `scanf(" %c", &c)` แทน (ต้องพิมพ์แล้วกด Enter)

## 💡 ให้ลองทำ

- ลบ `#ifndef GAME_H` ใน game.h แล้ว `#include "game.h"` ซ้ำสองครั้งใน main.c ดู error
- แก้ `getPlayerInput()` ใน render.c ให้กลับไปใช้ `scanf(" %c", &c)` แทน `getch()` แล้วสังเกตว่าต้องกด Enter ทุกครั้งต่างจากเดิมยังไง
- คอมเมนต์บรรทัด `system("cls")` ใน renderGame() ออก แล้วดูว่าจอเป็นยังไงเมื่อไม่ล้างจอทุก frame
- เปิด `dungeon_save.bin` ด้วย text editor — เห็นว่าคนอ่านไม่ได้ (binary)
