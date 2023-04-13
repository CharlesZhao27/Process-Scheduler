//
// Created by Weibo Zhao on 7/4/2023.
//
#include "linkedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

list_t *createList(){
    list_t *newList = malloc(sizeof(list_t));
    assert(newList);
    newList->head = newList->foot = NULL;
    return(newList);
}


list_t *insertToList(list_t *list, node_t *data){
    if(list->foot == NULL){
        list->head = list->foot = data;
    }else{
        list->foot->next = data;
        list->foot = data;
    }
    return list;
}

void printList(list_t *list){
    int node_num = 0;
    node_t *curr = list->head;
    if(curr == NULL){
        printf("Nothing in this List at this moment\n");
    }else{
        printf("[Num] name arrive_time service_time memory_required remain finished\n");
        while(curr != NULL){
            printf("[%d] %2s %3d %3d %3d %3d %3d %3d\n", node_num,
                   curr->name, curr->arrive,
                   curr->service,
                   curr->memory,
                   curr->remain,
                   curr->finished,
                   curr->status);
            node_num++;
            curr = curr->next;
        }
    }
}

node_t *popNode(list_t *list){
    if(list->head == NULL) return NULL;
    node_t *temp = list->head;
    if(list->head == list->foot){
        list->head = list->foot = NULL;
    }else{
        list->head = list->head->next;
    }
    temp->next = NULL;
    return temp;
}

int countNode(list_t *list){
    if(list->head == NULL){
        return(0);
    }else{
        int count = 0;
        node_t *head = list->head;
        while(head != NULL){
            count++;
            head = head->next;
        }
        return count;
    }
}

void deleteRunningNode(list_t *list){
    node_t *temp = NULL;
    node_t *curr = list->head;
    while(curr != NULL){
        if(curr->status == RUNNING && temp != NULL){
            temp->next = curr->next;
            free(curr);
        }else if(curr->status == RUNNING && temp == NULL){
            list->head = curr->next;
        }
        temp = curr;
        curr = curr->next;
    }
}

node_t *copyNode(node_t *dest, node_t *src){
    dest->arrive = src->arrive;
    strcpy(dest->name,src->name);
    dest->service = src->service;
    dest->memory = src->memory;
    dest->remain = src->remain;
    dest->status = src->status;
    dest->next = NULL;
    return dest;
}

node_t *findRunningNode(list_t *ready_queue){
    if(ready_queue->head == NULL){
        return NULL;
    }
    node_t *head = ready_queue->head;
    while(head != NULL){
        if(head->status == RUNNING) return head;
        head = head->next;
    }
    return NULL;
}

void printNode(node_t *node){
    printf("[%s] %d %d %d %d\n", node->name, node->arrive,
           node->status, node->remain, node->service);
}

int count_remain(list_t *ready_queue){
    int count = 0;
    struct node *curr = ready_queue->head;
    while(curr != NULL){
        if(curr->status == READY){
            count++;
        }else if(curr->status == RUNNING && curr->remain != 0){
            count++;
        }
        curr=curr->next;
    }
    return count;
}

void freeList(list_t *ready_queue){
    node_t *curr = ready_queue->head;
    node_t *temp;
    while(curr!=NULL){
        temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(ready_queue);
}

node_t *popThisNode(list_t *list, node_t *node){
    node_t *temp = list->head;
    node_t *prev;
    while(temp != NULL){
        if(strcmp(temp->name, node->name)==0){
            if(list->head == temp && list->head != list->foot){
                list->head = temp->next;
                temp->next = NULL;
                return(temp);
            }else if(list->head == temp && list->head == list->foot){
                list->head=list->foot=NULL;
                return(temp);
            }else{
                prev->next = temp->next;
                temp->next = NULL;
                return(temp);
            }
        }
        prev = temp;
        temp = temp->next;
    }
    return NULL;
}