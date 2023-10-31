#include <stdio.h>

// int strcmp(char *s, char *t){
//     int res = 0;
//     int i;
//     for(i = 0; *(s+i) != '\0' && *(t+i) != '\0'; i++){
//         if(*(s+i) - *(t+i) != 0){
//             return *(s+i) - *(t+i);
//         }else{
//             continue;
//         }
//     }
//     return 0;
// }






int main(int argc, char* argv[]){
    int a = 5;
    char *s = "Hello Gaais!"; //strings like this are a pointer by default so you can pass its variable and you know its an address
    //strings when defined like this are immutable, if u gon modify delcare like char[] str_arr
    char c = 'C';
    printf("Hello World!\n");
    printf("%d, %c\n", a, *s);
    printf("%d, %s\n", a, s);
    printf("%c\n", c);
    printf("a: %p, &s: %p, c: %p\n", &a, &s, &c);
    printf("a: %p, s: %p, c: %p\n", &a, s, &c);
    printf("a: %p, s[0]: %p, c: %p\n", &a, &s[0], &c);
    printf("a: %p, s[1]: %p, c: %p\n", &a, &s[1], &c);

    char s_arr[] = "Hello Gaays!"; //even tho we initialise like this, s_arr is a pointer
    char is_arr_conv_to_str[] = {'A', 'B', 'C', ' ','U', ' ', 'G', 'A', 'Y', '\0'}; //even tho we initialise like this, is_arr_conv_to_str is a pointer
    printf("%s\n", s_arr);
    printf("%s\n", is_arr_conv_to_str);
    s_arr[0] = 'G'; 
    printf("%s\n", s_arr);

    printf("Address of s_arr: %p\n", (void*)s_arr);  // Using array's name without index gives address of the first element
    printf("Address of is_arr_conv_to_str: %p\n", (void*)is_arr_conv_to_str);  // Same here

    for(int i = 0; s_arr[i] != '\0'; i++) {
    printf("s_arr[%d] = %c is stored at address: %p\n", i, s_arr[i], (void*)&s_arr[i]);
    /**
     * Using (void*) to cast addresses when using the %p format specifier in printf ensures type safety, 
     * portability, and compliance with the expected format, reducing the risk of undefined behavior and compiler warnings. 
     * This practice is a good habit, especially when writing code intended for use across various platforms and compilers.
    */
   float f = 8.000;
   printf("%d",f);

    }



    return 0;
}