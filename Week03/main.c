#include <stdio.h>

void swap_wrong(int a , int b){
	int temp;
	temp = a;
	a = b;
	b = temp;

	// Gets swapped inside here but not outside
	}

void swap(int *a , int *b){
// why will temp not work when stored as *temp (pointer) ?
// Making temp a pointer will make it store the address of *a
// We are not interested in the address of a rather the value being stored at a
int temp; // does not need to be a pointer as int can store addresses
temp = *a; //* - dereference operator
*a = *b;// Read as - value at a = value at b
*b = temp;
}

int main(int argc, char* argv[]){
    int a = 1;
    int b = 2;

    printf("before swap_wrong: a = %d, b = %d\n", a, b);
    printf("before swap: &a = %p, &b = %p\n", &a, &b);

    swap_wrong(a, b);
    printf("after swap_wrong: a = %d, b = %d\n", a, b);
    printf("after swap_wrong: &a = %p, &b = %p\n", &a, &b);

    swap(&a, &b);
    printf("after swap: a = %d, b = %d\n", a, b);
    printf("after swap: &a = %p, &b = %p\n", &a, &b);

    return 0;
}