#include <stdio.h>
#define INVENTORY_SIZE 10
#define EMPTY -1
void addItem(int inv[], int itemID) {
for (int i = 0; i < INVENTORY_SIZE; i++) {
if (inv[i] == EMPTY) {
inv[i] = itemID; // ใส่ใน slot ว่างแรกที่เจอ
return;
}
}
printf("Inventory full! Cannot add item %d\n", itemID);
}
void removeItem(int inv[], int itemID) {
    for (int i = 0; i < INVENTORY_SIZE; i++) {
if (inv[i] == itemID) {
inv[i] = EMPTY; // ลบ item แรกที่เจอ
return;
}
}
}
void displayInventory(int inv[]) {
printf("Inventory: ");
for (int i = 0; i < INVENTORY_SIZE; i++) {
if (inv[i] != EMPTY) {
printf("%d ", inv[i]);
}
}
printf("\n");
}
int main() {
int inventory[INVENTORY_SIZE];
for (int i = 0; i < INVENTORY_SIZE; i++) {
inventory[i] = EMPTY; // เริ่มต้นทุก slot ให้ว่างก่อนใช้งาน
}
int choice, itemID;
do {
displayInventory(inventory);
printf("1) Add item 2) Remove item 3) Quit\n");
printf("Choice: ");
scanf("%d", &choice);
if (choice == 1) {
printf("Item ID to add: ");
scanf("%d", &itemID);
addItem(inventory, itemID);
} else if (choice == 2) {
printf("Item ID to remove: ");
scanf("%d", &itemID);
removeItem(inventory, itemID);
}
} while (choice != 3);
return 0;
}