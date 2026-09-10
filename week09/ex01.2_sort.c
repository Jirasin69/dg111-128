#include <stdio.h>
#define MAX_SIZE 100
int main() {
int arr[MAX_SIZE];
int n;
printf("n: ");
scanf("%d", &n);
for (int i = 0; i < n; i++) scanf("%d", &arr[i]);
printf("Before: ");
for (int i = 0; i < n; i++) printf("%d ", arr[i]);
printf("\n");
// bubble sort — nested loop 2 ชั้น
for (int i = 0; i < n - 1; i++) {
for (int j = 0; j < n - i - 1; j++) {
if (arr[j] > arr[j+1]) {
int temp = arr[j];
arr[j] = arr[j+1];
arr[j+1] = temp;
}
}
}
printf("After: ");
for (int i = 0; i < n; i++) printf("%d ", arr[i]);
printf("\n");
return 0;
}