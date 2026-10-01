# Week 12 — Terminal Game Architecture (Dungeon Explorer)

ตัวอย่างประกอบ [slide_spec_week12.md](../slide_spec_week12.md) — เกมเดียวกัน สองแบบไฟล์ ไว้เทียบให้เห็นความต่าง

| โฟลเดอร์ | โครงสร้าง | ใช้ตอนไหน |
|----------|-----------|------------|
| [dungeon_multi_file/](dungeon_multi_file/) | แยก `game.h/c`, `render.h/c`, `save.h/c`, `main.c` | เดโมหลักของสัปดาห์นี้ — ตรงกับ slide ทุกหัวข้อ |
| [dungeon_single_file/](dungeon_single_file/) | รวมทุกอย่างไว้ใน `dungeon.c` ไฟล์เดียว | เทียบให้เห็นว่าทำไมถึงต้องแยกไฟล์ (PART 4: Multi-File Architecture) |

ทั้งสองโฟลเดอร์เล่นเหมือนกันทุกประการ (W/A/S/D เดิน, ชน `E` = ต่อสู้อัตโนมัติ, ถึง `>` = ลงชั้นถัดไป, V บันทึกเกม, Q ออก) — รายละเอียดการคอมไพล์และ exercise อยู่ใน README ของแต่ละโฟลเดอร์

## 🕹️ ตัวอย่างเกม Terminal เพิ่มเติม

ตัวอย่างเกมตาม [Mini Project — เกณฑ์และแนวทางการพัฒนา](../../../06-project-presentation/01-mini_project_guidelines.md) (4 เกมแรก) และเกมเพิ่มเติม เขียนด้วย C ล้วน (Windows Console: `conio.h`, `windows.h`) ไฟล์เดียว `main.c` ทุกเกม

| โฟลเดอร์ | เกม | วิธีเล่น |
|----------|-----|----------|
| [flappy_bird/](flappy_bird/) | Flappy Bird | กด Space บินผ่านช่องท่อ เก็บคะแนนสูงสุด |
| [pingpong/](pingpong/) | Ping Pong | เล่นกับ CPU ชนะก่อนได้ 2 แต้ม |
| [snake/](snake/) | Snake | บังคับงูกินอาหาร ยิ่งกินยิ่งเร็ว (เวอร์ชันโครง มี `TODO` ให้เติม) |
| [tetris/](tetris/) | Tetris | ต่อบล็อกให้เต็มแถว พร้อมระดับความเร็ว |
| [racing_drag_n_drive/](racing_drag_n_drive/) | Drag 'n' Drive | บังคับรถหลบสิ่งกีดขวางที่ตกลงมา มี 3 ชีวิต |
| [hangman/](hangman/) | Hangman | ทายตัวอักษรของคำภาษาอังกฤษให้ครบก่อนชีวิต 6 ครั้งหมด (ต้องรันในโฟลเดอร์ที่มี `words.txt`) |
| [sudoku/](sudoku/) | Sudoku | กรอกตัวเลขบนกระดาน 9×9 ที่สุ่มใหม่ทุกครั้ง |
| [tic_tac_toe/](tic_tac_toe/) | Tic-Tac-Toe | 2 ผู้เล่นสลับกันวาง X/O ให้ครบ 3 ช่องติดกันก่อน |
| [typing_game/](typing_game/) | Typing Game | พิมพ์คำที่ตกลงมาให้ถูกก่อนถึงพื้น ฝึกความเร็วการพิมพ์ |

คอมไพล์ในโฟลเดอร์ของแต่ละเกม:

```bash
gcc -std=c99 -Wall main.c -o main
```

> ใช้เป็นแรงบันดาลใจเท่านั้น — Mini Project ควรมีไอเดียหรือฟีเจอร์เพิ่มเติมของกลุ่มเอง ไม่ใช่ลอกตัวอย่าง
