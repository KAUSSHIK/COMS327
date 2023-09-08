//HEADER FILE IS ONLY MEANT TO BE READ ONCE
//HEADER GUARDS
#ifndef QUEUE_H    //all caps, replace "." with "_"    //if not defined then define
#define QUEUE_H

struct queue_node {
    struct queue_node *next;
    int data;
};


struct queue
{   //LINKED LIST BASED QUEUE
    struct queue_node *front, *back;
    int size;
};

//What is the difference between *i adn &i?
// *i is the value of i
// &i is the address of i

int queue_init(struct queue *q); //pass by address to make changes globally
int queue_destroy(struct queue *q);
int queue_insert(struct queue *q, int i);
int queue_remove(struct queue *q, int *i);
int queue_front(struct queue *q, int *i);
int queue_size(struct queue *q, int *i);
int is_empty(struct queue *q);


#endif