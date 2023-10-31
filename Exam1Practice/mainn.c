#include <stdio.h>
#include<string.h>

int main(int argc, char* argv[]){
    char a[] = "Penguin";
    printf("%s\n", a);
    char* b = "LOUDE";
    strcpy(a, b);
    printf("%s\n", a);
    strcpy(b, a);
     printf("%s\n", b);
    return 0;
}