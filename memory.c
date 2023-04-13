//
// Created by charles on 10/04/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "memory.h"
#include "linkedList.h"

memory_t *creatMemoryNode(void){
    memory_t *memoryList = malloc(sizeof(memory_t));
    assert(memoryList);
    memoryList->occupation = HOLE;
    memoryList->starts=0;
    memoryList->length=2048;
    memoryList->next=NULL;
    strcpy(memoryList->procName,"Nothing");
    return memoryList;
}



void mergeHoles(memory_t *head){
    memory_t *temp;
    while(head != NULL && head->next !=NULL){
        if(head->occupation == HOLE && head->next->occupation == HOLE){
            temp = head->next;
            head->length += temp->length;
            head->next = temp->next;
            free(temp);
        }
        head = head->next;
    }
}

int chooseBestFit(memory_t *head, node_t *process) {
    while (head != NULL) {
        if (head->occupation == HOLE && head->length > process->memory) {
            //slice head and allocate space for process
            memory_t *temp = creatMemoryNode();
            temp->length = head->length - process->memory;
            temp->starts = head->starts + process->memory;
            temp->occupation = HOLE;
            temp->next = head->next;
            head->next = temp;
            head->length = process->memory;
            strcpy(head->procName, process->name);
            head->occupation = PROCESS;
            return (head->starts);
        } else if (head->occupation == HOLE && head->length == process->memory) {
            //allocate this block to process
            strcpy(head->procName, process->name);
            head->occupation = PROCESS;
            return (head->starts);
        } else {
            head = head->next;
        }
    }
    return (-1);
}

void freeProcess(memory_t *head, node_t *node){
    while(head != NULL){
        if(strcmp(head->procName,node->name)==0){
            head->occupation = HOLE;
        }
        head=head->next;
    }
}

void freeMemoryList(memory_t *head){
    memory_t *temp;
    while(head!=NULL){
        temp = head;
        head = head->next;
        free(temp);
    }
}