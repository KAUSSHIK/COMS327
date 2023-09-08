#include "queue.h" //for our headers
#include <stdio.h>
#include <stdlib.h>

int queue_init(struct queue *q){
    q -> front = NULL;
    q -> back = NULL;
    q -> size = 0;
}

int queue_destroy(struct queue *q){
    struct queue_node *n;
    for(n = q->front; n; n = q->front){
        q->front = n->next;
        free(n);
    }
    q->back = NULL;
    q->size = 0;
    return 0;
}

int queue_insert(struct queue *q, int i){
    struct queue_node *n;

    if (!(n = malloc(sizeof(struct queue_node)))) {
        return -1;
    }
    if(!q->front){
        q->front = q->back = n;
    }else{
        q->back->next = n;
        q->back = n;
    }
    n->next  = NULL;
    n->data = i;

    return 0;
}
    
int queue_remove(struct queue *q, int *i){
    struct queue_node *n;
    if(!q->front){
        return -1;
    }
    n = q->front;
    if(!(q->front = q->front->next)){  //keep track of head, check if its empty and if it is then set it to null
        q->back = NULL;
    }
    *i = n -> data; //grab data out of it
    q -> size --;
    free(n); //free the node and its memory
    return 0;
}
int queue_front(struct queue *q, int *i){
    if(!q->front){
        return -1;
    }
    *i = q->front->data;
    return 0;
}

int queue_size(struct queue *q, int *i){
    return q->size;
}
int is_empty(struct queue *q){
    return ! (q->size);
}