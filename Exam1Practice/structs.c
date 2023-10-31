#include<stdio.h>
#include<strings.h>
#include<stdlib.h>

struct Person{
    char *name;
    int age;
    float height;
};

typedef struct Aadmi{
    char *name;
    int age;
    float height;
    struct Aadmi* dost;
}Aadmi;

int main(int argc, char* argv[]){

    // struct Person p1;

    // p1.name = malloc(50);

    // strcpy(p1.name, "Aryan Khyatsandra"); //can not refrence a char array like that ----> you gotta use strcpy
    // p1.age = 20;
    // p1.height = 6.1;
    // printf("Name: %s, Age: %d, Height: %f\n", p1.name, p1.age, p1.height);

    Aadmi a1;
    Aadmi a2;

    a1.name = malloc(50);
    a2.name = malloc(50);

    strcpy(a1.name, "Hrishi"); //can not refrence a char array like that ----> you gotta use strcpy
    a1.age = 20;
    a1.height = 5.9;
    a1.dost = &a2;
    printf("Name: %s, Age: %d, Height: %f\n", a1.name, a1.age, a1.height);

    strcpy(a2.name, "Rao"); //can not refrence a char array like that ----> you gotta use strcpy
    a2.age = 21;
    a2.height = 6.1;
    a2.dost = NULL;
    printf("Name: %s, Age: %d, Height: %f\n", a2.name, a2.age, a2.height);

    //LETS PRINT HRISHI'S DOST DETAILS
    printf("HRISHI FRIEND:\n");
    printf("Name: %s, Age: %d, Height: %f\n", (*a1.dost).name, a1.dost->age, a1.dost->height);
    
    a1.dost->age = 17;
    printf("Name: %s, Age: %d, Height: %f\n", a2.name, a2.age, a2.height);
    strcpy(a1.dost->name, "Raghsuram");
    printf("Name: %s, Age: %d, Height: %f\n", a2.name, a2.age, a2.height);



    return 0;
}