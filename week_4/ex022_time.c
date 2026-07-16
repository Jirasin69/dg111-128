#include <stdio.h>
int main(){
int time_seconds;
    printf("Enter time in seconds: ");
        scanf("%d", &time_seconds);
int hours = time_seconds / 3600;
    printf("hours:%d\n ", hours);
int minutes = (time_seconds % 3600) / 60;
    printf("minutes:%d\n ", minutes);
int seconds = time_seconds % 60;
    printf("seconds:%d", seconds);

printf("\n%02d:%02d:%02d\n", hours, minutes, seconds);
return 0;
}