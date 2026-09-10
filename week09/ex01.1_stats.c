#include <stdio.h>
#define MAX_SIZE 100
int main() {
int arr[MAX_SIZE];
int n;
printf("n: ");
scanf("%d", &n);
// วน loop รับค่า n ตัวเก็บใน arr[]
for (int i = 0; i < n; i++) {
scanf("%d", &arr[i]);
}
// หา min, max, sum ด้วย loop เดียว (traversal ครั้งเดียวพอ)
    int min = arr[0], max = arr[0], sum = 0;
    for (int i = 0; i < n; i++) {
    if (arr[i] < min) min = arr[i];
    if (arr[i] > max) max = arr[i];
    sum += arr[i];
}
    float average = (float)sum / n;
    printf("Min: %d\n", min);
    printf("Max: %d\n", max);
    printf("Sum: %d\n", sum);
    printf("Average: %.2f\n", average);
    return 0;
}