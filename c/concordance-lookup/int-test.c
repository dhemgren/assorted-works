#include <stdio.h>

int main(void){
    FILE *intprint;
    intprint = fopen("intprint.txt", "r+");
    int one = 255;
    int res = fwrite(&one, sizeof(int), 1, intprint);   
    printf("res %d\n", res);
    rewind(intprint);
    int read_value;
    fread(&read_value, sizeof(int), 1, intprint);
    printf("read_value (borde vara 10): %d\n", read_value);
    printf("position för filläsare: %ld\n", ftell(intprint));
}
