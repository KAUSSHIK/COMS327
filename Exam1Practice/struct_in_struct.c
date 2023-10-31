#include<stdio.h>

typedef struct{
    int x;
    int y;
}pair_of_pts;

typedef struct{
    pair_of_pts start;
    pair_of_pts end;
}Route;


int strcmp1(char s[], char t[]){
    int res = 0;
    int i;
    for(i = 0; s[i] != '\0' && t[i] != '\0'; i++){
        if(s[i] - t[i] != 0){
            return s[i] - t[i];
        }
    }
    return 0;
}

int strcmp2(char *s, char*t){

    int i;
    for(i = 0; s[i] == t[i]; i++){
        if(s[i] == '\0'){
            return 0;
        }
    }
    return s[i] - t[i];

}


int main(int argc, char* argv[]){
    Route r1;
    r1.start.x = 0;
    r1.start.y = 1;
    r1.end.x = 2;
    r1.end.y = 3;

    char s1[] = "bbc";
    char s2[] = "abc";

    int res = strcmp2(s1, s2);

    printf("%d\n", res);

    char s[20] = "Hello";
    char *const p = s;
    char *r = s;
    *p = 'M';
    *r = 'M';
    printf("%s\n", s);

    return 0;
}