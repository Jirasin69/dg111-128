#include <stdio.h>
#define MAX_N 20
void minMax(int arr[], int n, int *min, int *max)
{
    *min = arr[0];
    *max = arr[0];
    for (int i = 1; i < n; i++)
    {
        if (arr[i] < *min)
            *min = arr[i];
        if (arr[i] > *max)
            *max = arr[i];
    }
}
int main()
{
int n;
    printf("n (max %d): ", MAX_N);
    scanf("%d", &n);
int arr[MAX_N], lo, hi;
    for (int i = 0; i < n; i++)
    {
    printf("arr[%d]: ", i);
    scanf("%d", &arr[i]);
    }
    minMax(arr, n, &lo, &hi);
    printf("lo=%d, hi=%d\n", lo, hi);
return 0;
}