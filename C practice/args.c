#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    // printf("Hello World \n\r");
    // printf("My name is Mario \n\r");

    // printf("This is the argc %d \n\r",argc);
    // printf("This is the argv[0] %s \n\r",argv[0]);
    // printf("This is the argv[1] %s \n\r",argv[1]);

    int sum = atoi(argv[1]) + atoi(argv[2]);

    
    printf("This is the argv[0] %i \n\r", sum);

    
}