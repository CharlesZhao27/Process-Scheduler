#include "linkedList.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>


#define MAXSTRING 100
#define DOUBLE_MIN 0.0

struct arguments{
    char file_address[MAXSTRING];
    char scheduler[MAXSTRING];
    char memory_strategy[MAXSTRING];
    int quantum;
};

struct arguments *readInstructions(int argc, char** argv);
void readSubmissions(list_t *queue, FILE *file);
node_t *pickNextShortest(list_t *ready_queue);
node_t *pickNextRR(list_t *ready_queue, int time);
int exitProcessManager(list_t *submission, list_t *input_queue, list_t *ready_queue);
void printFinishMessage(list_t *input_queue, list_t *ready_queue, int time, node_t *current_running);
void printStats(list_t *ready_queue, int time);
int roundToInt(double d1);
void intToBE(uint8_t *arr, int time);



int main(int argc, char** argv){
    struct arguments *arguments = readInstructions(argc, argv);
    FILE *file = fopen(arguments->file_address,"r");
    assert(file);

    /* initialise three linked-list implemented queue */
    list_t *submission_queue = createList();
    list_t *input_queue = createList();
    list_t *ready_queue = createList();
    memory_t *memory = creatMemoryNode();


    /* read all process from submission file to submission queue */
    readSubmissions(submission_queue, file);

    /* start process manager Execution */
    int flag=1, time=0;
    while(flag){

        //check whether current running process exists and whether it's remaining time is 0
        node_t *curr = findRunningNode(ready_queue);
        if(curr != NULL && curr->remain <= 0){
            //print the result and terminate the process, Change STATUS to FINISHED, deallocate memory
            if(strcmp(arguments->memory_strategy,"best-fit")==0){
                //free allocated memory for the process
                freeProcess(memory,curr);
                mergeHoles(memory);
            }
            curr->finished = time;
            printFinishMessage(input_queue,ready_queue,time,curr);
            curr->status = FINISHED;
            curr = NULL;
        }

        //check whether process can enter input queue
        while(submission_queue->head != NULL && submission_queue->head->arrive <= time){
            //pop this process and insert it into input_queue
            node_t *input_node = popNode(submission_queue);
            insertToList(input_queue,input_node);
        }

        //check memory allocation strategy
        if(strcmp(arguments->memory_strategy,"infinite")==0){
            while(input_queue->head != NULL && input_queue->head->arrive <= time){
                node_t *ready_node = popNode(input_queue);
                ready_node->status = READY;
                insertToList(ready_queue, ready_node);
            }
        } else if(strcmp(arguments->memory_strategy,"best-fit")==0){
            node_t *temp = input_queue->head;
            while(temp != NULL){
                int memory_location = chooseBestFit(memory,temp);
                if(memory_location != -1){
                    popThisNode(input_queue,temp);
                    temp->status = READY;
                    printf("%d,READY,process_name=%s,assigned_at=%d\n",
                           time,temp->name,memory_location);
                    insertToList(ready_queue,temp);
                }
                temp = temp->next;
            }
        }

        //check scheduler algorithm
        if(strcmp(arguments->scheduler, "SJF")==0){
            if(curr != NULL){
                curr->remain-= arguments->quantum;
            }else if(curr == NULL && ready_queue->head != NULL){
                curr = pickNextShortest(ready_queue);
                if(curr != NULL){
                    curr->status=RUNNING;
                    printf("%d,RUNNING,process_name=%s,remaining_time=%d\n",
                           time,curr->name,curr->remain);
                    //TODO: TASK 4, create child process and passing argument to run ./process
                    uint8_t buffer[4];
                    intToBE(buffer, time); 
                    curr->remain-=arguments->quantum;
                }
            }
        } else if(strcmp(arguments->scheduler,"RR")==0){
            curr = pickNextRR(ready_queue, time);
            if(curr!=NULL) curr->remain -= arguments->quantum;
        }

        //check whether the process manager should terminate
        if(exitProcessManager(submission_queue,input_queue,ready_queue)==0){
            flag=0;
        } else time+=arguments->quantum;
    }

    printStats(ready_queue,time);
    fclose(file);
    free(input_queue);
    free(submission_queue);
    freeList(ready_queue);
    free(arguments);
    freeMemoryList(memory);
    return(0);
}





struct arguments *readInstructions(int argc, char** argv){
    struct arguments *input = malloc(sizeof(struct arguments));
    for(int i=0; i<argc; i++){
        if(strcmp(argv[i],"-f")==0){
            strcpy(input->file_address,argv[i+1]);
        }else if(strcmp(argv[i],"-s")==0){
            strcpy(input->scheduler,argv[i+1]);
        }else if(strcmp(argv[i],"-m")==0){
            strcpy(input->memory_strategy,argv[i+1]);
        }else if(strcmp(argv[i],"-q")==0){
            input->quantum = atoi(argv[i+1]);
        }
    }
    return(input);
}

void readSubmissions(list_t *queue, FILE *file){
    char name[MAXNAME], temp1[MAXDIGITS], temp2[MAXDIGITS], temp3[MAXDIGITS];
    while(fscanf(file, "%s %s %s %s", temp1, name, temp2, temp3)==4){
        struct node *process = malloc(sizeof(node_t));
        assert(process);
        process->arrive = atoi(temp1);
        strcpy(process->name, name);
        process->service = atoi(temp2);
        process->memory = atoi(temp3);
        process->remain = process->service;
        process->status = BLOCKED;
        process->finished = 0;
        process->next = NULL;
        insertToList(queue,process);
    }
}

node_t *pickNextShortest(list_t *ready_queue){
    int shortest_job = INT_MAX;
    node_t *curr = ready_queue->head;
    node_t *nextRun = NULL;
    while(curr != NULL){
        if(curr->status != FINISHED && curr->service <= shortest_job){
            shortest_job = curr->service;
            nextRun = curr;
        }
        curr = curr->next;
    }
    return nextRun;
}

node_t *pickNextRR(list_t *ready_queue, int time){
    node_t *head = ready_queue->head;
    node_t *temp;
    int remain_proc = count_remain(ready_queue);
    while(head != NULL && remain_proc != 0){
        if(head->status == READY){
            head->status = RUNNING;
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n",
                   time,head->name,head->remain);
            return head;
        }else if(head->status != READY && head->next != NULL){
            //we need to put this node to the back of the linked-list and check again
            if(head->status == RUNNING && head->remain > 0){
                if(remain_proc == 1){
                    return head;
                }
                temp = popNode(ready_queue);
                temp->status = READY;
                insertToList(ready_queue,temp);
                return pickNextRR(ready_queue, time);
            }else if(head->status == FINISHED){
                temp = popNode(ready_queue);
                insertToList(ready_queue,temp);
                return pickNextRR(ready_queue, time);
            }
        }else if(head->status == RUNNING && head->next == NULL){
            return head;
        }
    }
    return NULL;
}

int exitProcessManager(list_t *submission, list_t *input_queue, list_t *ready_queue){
    if(submission->head == NULL && input_queue->head == NULL){
        node_t *curr = ready_queue->head;
        while(curr!=NULL){
            if(curr->status != FINISHED){
                return(EXIT_FAILURE);
            }
            curr=curr->next;
        }
        return(EXIT_SUCCESS);
    }
    return(EXIT_FAILURE);
}

void printFinishMessage(list_t *input_queue, list_t *ready_queue,
                        int time, node_t *current_running){
    int remain_process = 0;
    node_t *curr = ready_queue->head;
    while(curr != NULL){
        if(curr->status != FINISHED && curr->status != RUNNING){
            remain_process++;
        }
        curr = curr->next;
    }
    curr = input_queue->head;
    while(curr != NULL){
        remain_process++;
        curr = curr->next;
    }
    printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n",
           time,current_running->name,remain_process);
}

void printStats(list_t *ready_queue, int time){
    int total_turnaround=0, count=0, turnaround;
    double overhead_time, overhead_max= DOUBLE_MIN;
    double overhead_mean,turnaround_mean,total_overhead=DOUBLE_MIN;
    node_t *curr = ready_queue->head;
    while(curr!=NULL){
        turnaround= (curr->finished - curr->arrive);
        total_turnaround += turnaround;
        count++;
        overhead_time = (double)turnaround / (double)curr->service;
        total_overhead += overhead_time;
        if(overhead_time > overhead_max){
            overhead_max = overhead_time;
        }
        curr=curr->next;
    }
    turnaround_mean = (double)total_turnaround / (double)count;
    overhead_mean = total_overhead / (double) count;
    printf("Turnaround time %d\n", roundToInt(turnaround_mean));
    printf("Time overhead %.2f %.2f\n",overhead_max, overhead_mean);
    printf("Makespan %d\n",time);
}

int roundToInt(double d1){
    return (int)(d1+1.0);
}

void intToBE(uint8_t *arr, int time){
    uint32_t simulationTime = (uint32_t) time;
    arr[0] = (simulationTime >> 24) & 0xFF;
    arr[1] = (simulationTime >> 16) & 0xFF;
    arr[2] = (simulationTime >> 8) & 0xFF;
    arr[3] = (simulationTime) & 0xFF;
}
