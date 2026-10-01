# Week 13 — Raylib Game Architecture (Dungeon Explorer)

ตัวอย่างประกอบ [slide_spec_week13.md](../slide_spec_week13.md) — เกมเดียวกับ [Week 12](../../week_12/src/README.md) แต่เปลี่ยน render + input เป็น Raylib — มีสองแบบไฟล์ ไว้เทียบให้เห็นความต่าง

| โฟลเดอร์ | โครงสร้าง | ใช้ตอนไหน |
|----------|-----------|------------|
| [dungeon_multi_file/](dungeon_multi_file/) | แยก `game.h/c`, `render.h/c`, `save.h/c`, `main.c` | เดโมหลัก — เห็นชัดว่าแก้แค่ `render.c` + `main.c` ส่วน `game.c`/`save.c` เหมือน Week 12 ทุกบรรทัด |
| [dungeon_single_file/](dungeon_single_file/) | รวมทุกอย่างไว้ใน `dungeon_raylib.c` ไฟล์เดียว | เทียบกับ `dungeon.c` ของ Week 12 แบบบรรทัดต่อบรรทัด |

ทั้งสองโฟลเดอร์เล่นเหมือนกันทุกประการ (W/A/S/D หรือลูกศร เดิน, ชนสี่เหลี่ยมสีแดง = ต่อสู้อัตโนมัติ, ถึงช่องสีทอง = ลงชั้นถัดไป, V บันทึกเกม, Q ออก) — รายละเอียดการคอมไพล์และ exercise อยู่ใน README ของแต่ละโฟลเดอร์

## 🕹️ ตัวอย่างเกม Raylib เพิ่มเติม

ตัวอย่างเกมตาม [Mini Project — เกณฑ์และแนวทางการพัฒนา](../../../06-project-presentation/01-mini_project_guidelines.md) (3 เกมแรก) และเกมเพิ่มเติม — ไฟล์เดียว `main.c` ทุกเกม · `arkanoid`, `snake`, `tetris`, `platformer` เป็น sample จาก raylib (zlib/libpng license) · `nes_*` มาจากชุดตัวอย่างสไตล์ NES ของวิชา (จอเกม 320×240 ขยาย ×2.5 เป็น 800×600)

| โฟลเดอร์ | เกม | วิธีเล่น |
|----------|-----|----------|
| [arkanoid/](arkanoid/) | Arkanoid | เลื่อนแท่นรับลูกบอลให้ชนบล็อกจนหมด มี 5 ชีวิต |
| [snake/](snake/) | Snake | งูกินผลไม้บนตาราง ห้ามชนขอบและชนตัวเอง |
| [tetris/](tetris/) | Tetris | ต่อบล็อก 7 แบบให้เต็มแถว มีช่องแสดงชิ้นถัดไป |
| [platformer/](platformer/) | Platformer | วิ่ง-กระโดดบนแท่นเก็บเหรียญให้ครบ 10 เหรียญ — ตัวอย่าง delta time + Camera2D |
| [nes_gameloop/](nes_gameloop/) | NES Shooter | ยิงศัตรูที่ตกลงมา — ตัวอย่าง FSM `TITLE → PLAYING → GAME_OVER` + bullet/enemy pool ([concepts.md](nes_gameloop/concepts.md) สรุปความรู้รายสัปดาห์ที่ใช้) |
| [nes_tictactoe/](nes_tictactoe/) | Tic-Tac-Toe N×N | เลือกขนาดกระดานและจำนวนหมากที่ต้องเรียงเพื่อชนะได้เอง — เมาส์ + คีย์บอร์ด |

คอมไพล์ในโฟลเดอร์ของแต่ละเกม (เปลี่ยน `<game>` เป็นชื่อเกม):

```bash
gcc -std=c99 main.c -o <game> -lraylib -lm -lwinmm -lgdi32
```

> ใช้เป็นแรงบันดาลใจเท่านั้น — Mini Project ควรมีไอเดียหรือฟีเจอร์เพิ่มเติมของกลุ่มเอง ไม่ใช่ลอกตัวอย่าง
