#include <stdio.h>

int main(int argc, char* argv[]){
    
    int p[][2] = {{1,2}, {3,99}, {5,77}};
    printf("%d\n", **p); //1
    printf("%d\n", *(*(p+1)+1)); //99
    printf("%d\n", *((*p+1)+1)); //3
    printf("%d\n", *(*p+1)); //2

    // p + 1 ===> row + 1
    // *p + 1 =====> curr. element + 1

    printf("%d\n", *(*(p+2)+1)); //77


    return 0;
}