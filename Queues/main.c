#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main(int argc, char *argv[])
{
    struct queue *q;
    int i;

    queue_init(&q);

    for(i = 0; i < 100; i++){
        queue_insert(&q, i);
    }
}