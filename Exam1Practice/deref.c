#include <stdio.h>
// Dereferencing: Accessing or modifying the value stored at the memory location that a pointer is pointing to.
int main(int argc, char *argv[])
{

    int x = 10;             
    int *p = &x;              
    printf("Value of x: %d\n", x);
    printf("Address of x: %p\n", (void *)&x);
    printf("Address of x: %p\n", p);

    *p = 20;          
    printf("AFTER UPDATE, value of x: %d\n", x);
    printf("AFTER UPDATE, address of x: %p\n", (void*)&x); 

    x = 30;
    printf("AFTER UPDATE, value of x: %d\n", *p); 


    int *t = p;
    printf("%d\n", *t);

    int **s = &p;
    printf("%d\n", **s);
    printf("%p\n", *s);
    printf("Address of p stored in s: %p\n", s);
    printf("Will print the address of x: %p\n", &x); // Will print the address of x
    printf("Will print the address of p: %p\n", &p); // Will print the address of p
    printf(" Will print the address stored in p, which should be the same as the address of x: %p\n", p);  // Will print the address stored in p, which should be the same as the address of x

    




    return 0;
}
