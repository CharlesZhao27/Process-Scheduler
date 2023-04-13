//
// Created by charles on 10/04/23.
//

#ifndef WEIBOZ_PROJECT1_MEMORY_H
#define WEIBOZ_PROJECT1_MEMORY_H

#include "linkedList.h"

#define PROCESS 0
#define HOLE 1

typedef struct memory memory_t;
struct memory{
    int occupation;
    int starts;
    int length;
    char procName[8];
    memory_t *next;
};
//initialize a simulated memory implemented by linked-list
memory_t *creatMemoryNode(void);
//merge any adjacent holes to one segment
void mergeHoles(memory_t *head);
//choose the best-fit contiguous block of memory, return NULL if it's fail to allocate
int chooseBestFit(memory_t *head, node_t *process);
//deallocate memory for terminated process
void freeProcess(memory_t *head, node_t *node);
//free memory list
void freeMemoryList(memory_t *head);

#endif //WEIBOZ_PROJECT1_MEMORY_H
