#include <stdio.h>
#include <stdlib.h>

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

/**
 * The strdup() function returns a pointer to a new string which is a
duplicate of the string s. Memory for the new string is obtained with
malloc(3), and can be freed with free(3).
The strndup() function is similar, but copies at most n bytes. If s is
longer than n, only n bytes are copied, and a terminating null byte
('\0') is added.
*/
//char is returned because: C does not have a string data type. To represent a string, a set of characters is enclosed within double quotes (" "). C converts to char array
char *strndup ( const char *s , size_t n )
{
    char *t = malloc ( (n + 1) * sizeof(char) ) ; // +1 for '\0' 
    int i;

    if(!t) return NULL;

    for(i = 0; i <n && s[i]; i++){
        t[i] = s[i];
    }
    t[i] = '\0';
    return t ;
}
//what is size_t?
//size_t is an unsigned integer data type which is defined in various header files such as <stdio.h>, <stdlib.h>, <string.h>, <time.h> etc. It is used to represent the size of an object. Library functions that take or return sizes expect them to be of type or have the return type of size_t. Also, the most appropriate type for the result of the sizeof operator is size_t.

int main(int argc, char* argv[]){
    int a = 1;
    int b = 2;
    printf("a = %d, b = %d\n", a, b);

    swap_wrong(a, b);
    printf("WRONG");
    printf("a = %d, b = %d\n", a, b);

    // swap(a,b);
    // printf("CORRECT BUT WRONG");
    // printf("a = %d, b = %d\n", a, b);

    swap(&a,&b);
    printf("CORRECT");
    printf("a = %d, b = %d\n", a, b);


    /* 0 1 2 *
* 012345678901234567890123456 */
char s [] = "On Wednesdays we wear pink!" ;
*( s + 1) = 'O';
*( s + 2) = 'n';
*( s + 3) = ' ';
*( s + 4) = 'S';
*( s + 5) = 'a';
*( s + 6) = 't';
*( s + 7) = 'u';
*( s + 8) = 'r';
*( s + 26) = '.';
//print to see s
printf("%s\n", s+1);


char * a [] = {
" happen  " , " to " , " make " ,
" stop " , " \" fetch \" " , " trying " ,
};
int o[] = { 3 , 5 , 1 , 2 , 4 , 0 };
for (int i = 0; i < 6; i ++) {
printf ( "%s" , a[o[i]]);
}

    return 0;
}