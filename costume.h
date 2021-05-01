/*
 * Shyue Shi Leong and Ze Jia Lim
 *
 * CS 441/541: Pirates and Ninjas (Project 5)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "semaphore_support.h"

/*****************************
 * Defines
 *****************************/


/*****************************
 * Structures
 *****************************/
typedef struct pirate{
    int entry;
    int left;
}pirate;

typedef struct ninja{
    int entry;
    int left;
}ninja;

typedef struct Node{
    semaphore_t data;
    struct Node* next;
    struct Node* prev;
}Node;

typedef struct list{
    Node * head;
    Node * tail;
    int size;
}list;

/*****************************
 * Global Variables
 *****************************/
semaphore_t waitP;
semaphore_t waitN;
semaphore_t print;
semaphore_t pMutex;
semaphore_t nMutex;
semaphore_t pirateQ;
semaphore_t ninjaQ;
pirate * pirateArr;
ninja * ninjaArr;
int timer;
int numPirate;
int numNinja;
int numBooth;
int done;
int Pcount;
int Ncount;
int isPirate;
list * pirateQueue;
list * ninjaQueue;
int pCounter;
int nCounter;
int firstTime;
int firstTime2;

/*****************************
 * Function Declarations
 *****************************/
/*
 * This method describes the behavior of each 
 * pirate thread
 *
 * Parameters:
 *  tid : thread ID
 *
 * Returns:
 *  None
 */
void *pirateExec(void *threadid);

/*
 * This method describes the behavior of each
 * ninja thread
 *
 * Parameters:
 *  tid : thread ID
 * 
 * Returns: 
 *  None
 */
void *ninjaExec(void *threadid);

/*
 * This method is used to initialize the 
 * linked list
 *
 * Parameters:
 *  list : the list pointer to the linked list
 *
 * Returns:
 *  None
 */
void init_list(list * List);

/*
 * This method is used to add a new data to the 
 * end of the linked list
 *
 * Parameters:
 *  list : the list pointer to the linked list
 *  data : the new data to store in the linked list
 *
 * Returns:
 *  None
 */
void add(list * List, semaphore_t data);

/*
 * This method is used to remove the first data in 
 * lthe linked list
 *
 * Parameters:
 *  list : the list pointer to the linked list
 *
 * Returns:
 *  the data that is removed from the linked list
 */
semaphore_t removeNode(list * List);