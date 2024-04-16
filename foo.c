#include <stdio.h>

void printTriangle(int height) {
    for (int i = 1; i <= height; i++) {
        for (int j = 1; j <= i; j++) {
            printf("* ");
        }
        printf("\n");
    }
}

int main(){
    int height;
    do {
        printf("Enter the height of the triangle (1 <= height <= 9): ");
        scanf("%d", &height);
    } while (height <= 0 || height >= 10);
    
    printTriangle(height);
    printf("Press ENTER to exit...");
    getchar();
    getchar();
    return 0;
}