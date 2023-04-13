//
// Created by Weibo Zhao on 7/4/2023.
//

#ifndef COMP30023_2023_PROJECT_1_LINKEDLIST_H
#define COMP30023_2023_PROJECT_1_LINKEDLIST_H

#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

#define MAXNAME 8
#define MAXDIGITS 10

/* definition of all process status */
#define BLOCKED (-1)
#define READY 0
#define RUNNING 1
#define FINISHED 2

/* Structure Definition */
typedef struct node node_t;
struct node{
    int arrive;
    char name[MAXNAME];
    int service;
    int memory;
    int remain;
    int status;
    int finished;
    node_t *next;
};

typedef struct list{
    node_t *head;
    node_t *foot;
}list_t;

/* function Definition */

//create an Empty linked-list
list_t *createList();
//create an Empty Node
node_t *createNode();
// append node to the end of the list
list_t *insertToList(list_t *list, node_t *data);
//printList
void printList(list_t *list);
//pop node
node_t *popNode(list_t *list);
//count Node
int countNode(list_t *list);
//delete Node
void deleteRunningNode(list_t *list);
//copy node
node_t *copyNode(node_t *dest, node_t *src);
//find RUNNING status Node
node_t *findRunningNode(list_t *ready_queue);
//print Node
void printNode(node_t *node);
//count remain process in ready queue
int count_remain(list_t *ready_queue);
//free List
void freeList(list_t *ready_queue);
//pop certain node from list
node_t *popThisNode(list_t *list, node_t *node);
#endif

#endif //COMP30023_2023_PROJECT_1_LINKEDLIST_H
