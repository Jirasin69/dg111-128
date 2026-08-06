#include <stdio.h>

int main() {
    int count = 1;
    for (int row = 1; row <= 3; row++) {
        printf("+---+---+---+---+---+\n");
        for (int col = 1; col <= 5; col++) 
        
        {
            
            if(count >= 10) {
                printf("|%d ", count);
            } else 
                printf("| %d ", count);
            count++;
            
        }
        printf("|\n");
    
    }
        printf("+---+---+---+---+---+\n");
        
        return 0;
    }
