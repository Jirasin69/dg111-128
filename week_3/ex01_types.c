#include <stdio.h>
int main() {
// ประกาศและกาหนดค่าตัวแปร
int age = 20;
float gpa = 3.75f;
double pi = 3.14159265358979;
char grade = 'A';
// แสดงค่าและขนาด
printf("int age = %d (size: %zu bytes)\n", age,
sizeof(age));
printf("float gpa = %.2f (size: %zu bytes)\n", gpa,
sizeof(gpa));
printf("double pi = %.10f (size: %zu bytes)\n", pi,
sizeof(pi));
printf("char grade = %c (size: %zu bytes)\n", grade,
sizeof(grade));
return 0;
}