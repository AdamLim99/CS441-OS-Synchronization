/*
 * Shyue Shi Leong and Ze Jia Lim
 *
 * CS 441/541: Pirates and Ninjas (Project 5)
 *
 */
#include "costume.h"

int main(int argc, char * argv[]) {

    // initializing the local variables
    pthread_t * pirates;
    pthread_t * ninjas;
    int i;
    char * ptr;

    // display an rror message if there are not enough or too many
    // command line arguments, then exit the program
    if(argc<2 || argc>5){
	fprintf(stderr,"Invalid number of arguments! Please enter 2-5 arguments.\n");
	exit(-1);
    }

    // display an error message if time to live is less than 1, then exit the program
    timer = strtol(argv[1],&ptr,10);
    if(timer <= 0){
	fprintf(stderr,"*Invalid time to live! Time to live should be >= 1.\n");
	exit(-1);
    }

    // display an error message if number of pirates is <= 0, then exit the program
    if(argc<6 && argc>2){
	numPirate = strtol(argv[2],&ptr,10);
	if(numPirate<=0){
	    fprintf(stderr,"Invalid number of pirates! Number of pirates should be >0.\n");
	    exit(-1);
	}
    }
    // set the number of pirates to default number of 5 if no argument is provided
    else{
	numPirate=5;
    }

    // display an error message if number of ninjas is <= 0, then exit the program
    if(argc<6 && argc>3){
	numNinja = strtol(argv[3],&ptr,10);
	if(numNinja<=0){
	    fprintf(stderr,"Invalid number of ninjas! Number of ninjas should be >0.\n");
	    exit(-1);
	}

    }
    // set the number of ninjas to default number of 5 if no argument is provided
    else{
  	numNinja=5;
    }

    // display an error message if number of booths is <=0, then exit the program
    if(argc<6 && argc>4){
	numBooth = strtol(argv[4],&ptr,10);
	if(numBooth<=0){
	    fprintf(stderr,"Invalid number of costume booths! Number of costume booths should be >0.\n");
	    exit(-1);
	}

    }
    // set the number of booths to default number of 2 if no argument is provided
    else{
  	numBooth=2;
    }

    // dynamically allocate space for all the data sturctures and initialize all
    // global variables
    pirates = (pthread_t *)malloc(sizeof(pthread_t) * numPirate);
    ninjas = (pthread_t *)malloc(sizeof(pthread_t) * numNinja);
    pirateArr = (pirate *)malloc(sizeof(pirate) * numPirate);
    ninjaArr = (ninja *)malloc(sizeof(ninja) * numNinja);
    done=0;
    pCounter=0;
    nCounter=0;
    Pcount=0;
    Ncount=0;
    firstTime=0;
    firstTime2=0;
    isPirate=1;

    // print the starting information
    printf("Time To Live (seconds) : %d\n",timer);
    printf("Number of Pirates      : %d\n",numPirate);
    printf("Number of Ninjas       : %d\n", numNinja);
    printf("---------------------------------\n");
    printf(" Thread ID  |   Action\n");
    printf("------------+-------------------------------------------\n");
    
    //create mutex and initializing the list
    semaphore_create(&waitP, 1); 
    semaphore_create(&waitN, 1);
    semaphore_create(&print, 1);
    semaphore_create(&pirateQ, 0);
    semaphore_create(&ninjaQ, 0);
    semaphore_create(&pMutex, numBooth);  
    semaphore_create(&nMutex, numBooth); 
    pirateQueue = (list *)malloc(sizeof(list));
    ninjaQueue = (list *)malloc(sizeof(list)); 
    init_list(ninjaQueue);
    init_list(pirateQueue);
  
    // create the pirate threads
    for(i=0;i<numPirate;i++){
	pthread_create(&pirates[i], NULL, pirateExec, (void *)(intptr_t)i);
    	pirateArr[i].entry=0;
    	pirateArr[i].left=0;
    }

    // create the ninja threads
    for(i=0;i<numNinja;i++){
   	pthread_create(&ninjas[i], NULL, ninjaExec, (void *)(intptr_t)i);
    	ninjaArr[i].entry=0;
    	ninjaArr[i].left=0;
    }

    // sleep for the amount of time for the threads to run and stop the threads
    // once the time is up
    sleep(timer);
    done=1;

    // waiting for the threads to return
    for(i=0;i<numPirate;i++){
	pthread_join(pirates[i], NULL);
    }
    for(i=0;i<numNinja;i++){
   	pthread_join(ninjas[i], NULL);
    }

    // printing the tabulated results
    printf("------------+-------------------------------------------\n");
    for(i=0;i<numPirate;i++){
	printf(" Pirate   %d : Entered   %d / Left   %d\n",i,pirateArr[i].entry,pirateArr[i].left);
    }
    for(i=0;i<numNinja;i++){
   	printf(" Ninja    %d : Entered   %d / Left   %d\n",i,ninjaArr[i].entry,ninjaArr[i].left);
    }
    printf("------------+-------------------------------------------\n");

    //cleanup   
    pthread_exit(NULL);
    semaphore_destroy(&waitP); 
    semaphore_destroy(&waitN);
    semaphore_destroy(&print);
    semaphore_destroy(&pirateQ);
    semaphore_destroy(&ninjaQ);
    semaphore_destroy(&pMutex);  
    semaphore_destroy(&nMutex); 
    free(pirates);
    free(ninjas);
    free(pirateArr);
    free(ninjaArr);

    return 0;
}

void *pirateExec(void *threadid){
    // initializing the local variables
    int tid = (intptr_t)threadid;
    int delay=0;
    int i=0;
    semaphore_t self;
    semaphore_t toReturn;
    semaphore_create(&self, 0);

    do{	
        // indicate that the pirate is waiting
	semaphore_wait(&print);
	printf(" Pirate   %d | Waiting\n",tid);
	semaphore_post(&print);

	// hold the pirates here if it is not their turn to go
	if(isPirate == 0){
	    semaphore_wait(&pirateQ);
	}
	
	semaphore_wait(&waitP);
	// if the number of pirates is less than the number of booth,
	// then we do not need to sleep them and just let them go when
 	// they are ready
	if(numPirate <= numBooth){  
	    semaphore_post(&waitP);
	}
	// Otherwise, we keep track of how many threads were sleeping 
	// and wake up the appropriate number of threads for them to enter 
	// the critical section	
	else{
	    Pcount++;
	    // this if condition will only enter once, which is when it is the pirates' first 
            // time to enter the booth
	    if(firstTime2 == 0){
	 	// allow the number of pirate threads through according to the
		// number of booth and add the rest of threads in the queue and sleep them
		if(Pcount > numBooth){
		    add(pirateQueue, self);
	    	    semaphore_post(&waitP);
	    	    semaphore_wait(&self);
		    if(done == 1){
			pthread_exit(NULL);
		    }
		    
		}
		else if(Pcount <= numBooth){
	    	    semaphore_post(&waitP);
		}
	    }
	    // threads will enter this else if clause for the rest of the program once it is not
            // pirates' first time to enter the booth anymore
	    else if(firstTime2==1){
		// we would add each thread in the list and sleep them except for the last one.
		// the last thread will wake the number of pirate threads according to the number 
		// of booth, then add itself to the list and sleep itself
		if(Pcount == numPirate){
		    add(pirateQueue, self);
		    for(i=0;i<numBooth;i++){
			toReturn = removeNode(pirateQueue);
	    	        semaphore_post(&toReturn);
		    }
		    semaphore_post(&waitP);
	    	    semaphore_wait(&self);
		    if(done == 1){
			pthread_exit(NULL);
		    }
		}
		else if(Pcount < numPirate){
		    add(pirateQueue, self);
		    semaphore_post(&waitP);
	    	    semaphore_wait(&self);
		    if(done == 1){
			pthread_exit(NULL);
		    }
		}
	    }
	}

	// here is where the pirates enter the costume department for costume preparation
	semaphore_wait(&pMutex);
	semaphore_wait(&waitP);
	// increment the counter to the number of times that pirate enters the costume department
	pirateArr[tid].entry = pirateArr[tid].entry + 1;
	semaphore_post(&waitP);
	semaphore_wait(&print);
	// indicate that the pirate is in the booth doing costume preparation
       	printf(" Pirate   %d | Costume preparation\n",tid);
	semaphore_post(&print);
	// sleep the thread for a random amount of time to simulating the thread doing 
	// costume preparation
	srandom(time(NULL));
	delay = random()%50000;
	usleep(delay);
	semaphore_post(&pMutex);
	// here is where the pirates exit the costume department

	semaphore_wait(&waitP);
	// increment the number of times the pirates left the costume department
	pirateArr[tid].left = pirateArr[tid].left + 1;
	semaphore_wait(&print);
	// indictae that the pirate is leaving the costume department
	printf(" Pirate   %d | Leaving\n",tid);
	semaphore_post(&print);
	// update the global variables
	isPirate=0;
	Pcount--;
	pCounter++;
	// unlocking the ninja threads and allowing them to continue execution
	if(pCounter == numBooth && numPirate >= numBooth){
            if(firstTime==0){
		firstTime=1;
		for(i=0;i<numNinja;i++){
		    semaphore_post(&ninjaQ);
		}
	    }
	    else{
		if(numNinja <= numBooth){
		    for(i=0;i<numNinja;i++){
		    	semaphore_post(&ninjaQ);
		    }
		}
		else{
		    for(i=0;i<numBooth;i++){
		    	semaphore_post(&ninjaQ);
		    }

		}
	    }
	    pCounter=0;
	}
	else if(pCounter == numPirate && numPirate < numBooth){
	    if(firstTime==0){
		firstTime=1;
		for(i=0;i<numNinja;i++){
		    semaphore_post(&ninjaQ);
		}
	    }
	    else{
		if(numNinja <= numBooth){
		    for(i=0;i<numNinja;i++){
		    	semaphore_post(&ninjaQ);
		    }
		}
		else{
		    for(i=0;i<numBooth;i++){
		    	semaphore_post(&ninjaQ);
		    }

		}
	    }
	    pCounter=0;
	}
	semaphore_post(&waitP);

	// sleep the pirate threads for a random amount of time to simulate them leaving for battle
	srandom(time(NULL));
	delay = random()%1000000;
	usleep(delay);
    }while(done==0);

    // wake any threads that are sleeping in the pirate queue and allow them to exit once the time
    // to live is up
    if(pirateQueue->size > 0){
	int temp = pirateQueue->size;
	for(i=0;i<temp;i++){
	    toReturn = removeNode(pirateQueue);
	    semaphore_post(&toReturn);
	}
    }

    semaphore_destroy(&self);
    pthread_exit(NULL);
}

void *ninjaExec(void *threadid){
    // initializing the local variables
    int tid = (intptr_t)threadid;
    int delay=0;
    int i=0;
    semaphore_t self;
    semaphore_t toReturn;
    semaphore_create(&self, 0);

    do{
	// indicate that the ninja is waiting
	semaphore_wait(&print);
	printf(" Ninja    %d | Waiting\n",tid);
	semaphore_post(&print);
	
	// hold the ninjas here if it is not their turn to go
	if(isPirate == 1){
	    semaphore_wait(&ninjaQ);
	}
	
	semaphore_wait(&waitN);
	// if the number of ninjas is less than the number of booth,
	// then we do not need to sleep them and just let them go when
 	// they are ready
	if(numNinja <= numBooth){
	    semaphore_post(&waitN);
	}
	// Otherwise, we keep track of how many threads were sleeping 
	// and wake up the appropriate number of threads for them to enter 
	// the critical section	
	else{
	    Ncount++;
	    // this if condition only enters when it is ninjas' first time to enter the booth
	    if(firstTime == 0){
		// allow the number of ninja threads through according to the
		// number of booth then add the rest of threads into the queue and sleep them
		if(Ncount > numBooth){
		    add(ninjaQueue, self);
	    	    semaphore_post(&waitN);
	    	    semaphore_wait(&self);
		    if(done == 1){
			pthread_exit(NULL);
		    }
		}
		else if(Ncount <= numBooth){
	    	    semaphore_post(&waitN);
		}
	    }
	    // thread will enter this else if clause if this is not ninjas' first time to enter
            // the booth anymore
	    else if(firstTime==1){
		// we would add each thread into the list and sleep them except for the last one.
		// the last thread will wake the number of ninja threads according to the number 
		// of booth then add itself to the list and sleep itself
		if(Ncount == numNinja){
		    add(ninjaQueue, self);
		    for(i=0;i<numBooth;i++){
			toReturn = removeNode(ninjaQueue);
	    	        semaphore_post(&toReturn);
		    }
		    semaphore_post(&waitN);
	    	    semaphore_wait(&self);
		    if(done == 1){
			pthread_exit(NULL);
		    }
		}
		else if(Ncount < numNinja){
		    add(ninjaQueue, self);
		    semaphore_post(&waitN);
	    	    semaphore_wait(&self);
		    if(done == 1){
			pthread_exit(NULL);
		    }
		}
	    }
	}

	// here is where the pirates enter the costume department for costume preparation
	semaphore_wait(&nMutex);
	semaphore_wait(&waitN);
	// increment the number of times the ninja enters the costume department
	ninjaArr[tid].entry = ninjaArr[tid].entry + 1;
	semaphore_post(&waitN);
	semaphore_wait(&print);
	// indicate that the ninjas are in the costume booth doing costume preparation
	printf(" Ninja    %d | Costume preparation\n",tid);
	semaphore_post(&print);
	// sleep the thread for a random amount of time to simulating the thread doing 
	// costume preparation
	srandom(time(NULL));
	delay = random()%50000;
	usleep(delay);
	semaphore_post(&nMutex);
	// here is where the ninjas exit the costume department
	
	semaphore_wait(&waitN);
	// increment the number of times the ninja left the costume preparation
	ninjaArr[tid].left = ninjaArr[tid].left + 1;
	semaphore_wait(&print);
	// indicate the ninja is leaving the costume department
	printf(" Ninja    %d | Leaving\n",tid);
	semaphore_post(&print);
	// update global variables
    	isPirate=1;
	Ncount--;
	nCounter++;
	// unlocking the pirate threads to allow them to continue execution
	if(nCounter == numBooth && numNinja >= numBooth){
            if(firstTime2==0){
	 	firstTime2=1;
		for(i=0;i<numPirate;i++){
		    semaphore_post(&pirateQ);
		}
	    }
	    else{
		if(numPirate <= numBooth){
		    for(i=0;i<numPirate;i++){
		    	semaphore_post(&pirateQ);
		    }
		}
		else{
		    for(i=0;i<numBooth;i++){
		    	semaphore_post(&pirateQ);
		    }

		}
	    }
	    nCounter=0;
	}
	else if(nCounter == numNinja && numNinja < numBooth){
	    if(firstTime2==0){
		firstTime2=1;
		for(i=0;i<numPirate;i++){
		    semaphore_post(&pirateQ);
		}
	    }
	    else{
		if(numPirate <= numBooth){
		    for(i=0;i<numPirate;i++){
		    	semaphore_post(&pirateQ);
		    }
		}
		else{
		    for(i=0;i<numBooth;i++){
		    	semaphore_post(&pirateQ);
		    }

		}
	    }
	    nCounter=0;
	}
	semaphore_post(&waitN);
	
	// sleep the ninja threads for a random amount of time to simulate them leaving for battle
	srandom(time(NULL));
	delay = random()%1000000;
	usleep(delay);	
    }while(done==0);

    // wake any threads that are sleeping in the ninja queue and allow them to exit once the time
    // to live is up
    if(ninjaQueue->size > 0){
	int temp = ninjaQueue->size;
	for(i=0;i<temp;i++){
	    toReturn = removeNode(ninjaQueue);
	    semaphore_post(&toReturn);
	}
    }

    semaphore_destroy(&self);
    pthread_exit(NULL);
}

void init_list(list * List){
    // dynamically allocate space for the head and tail nodes of the list
    List->head = (struct Node*)malloc(sizeof(struct Node));
    List->tail = (struct Node*)malloc(sizeof(struct Node));

    // set the links of each node appropriately and set the size to 0
    List->head->next = List->tail;
    List->tail->prev = List->head;
    List->size=0;
}

void add(list * List, semaphore_t newData){
    // dynamically allocating space for the new node and put the data in it
    Node * newNode = (Node *)malloc(sizeof(Node));
    newNode->data = newData;

    // set the links of the new node and the adjacent nodes
    newNode->next = List->tail;
    newNode->prev = List->tail->prev;
    List->tail->prev->next = newNode;
    List->tail->prev = newNode;
   
    // increment the size of the list
    List->size++;
}

semaphore_t removeNode(list * List){
    // initializing local variables
    semaphore_t toReturn;
    Node * toRemove = List->head->next;

    // return NULL if there is nothing in the list
    if(toRemove == List->tail){
	return toReturn;
    }
    
    // remove the ndoe from the list and return the data in the node that is removed
    toReturn = toRemove->data;
    List->head->next = toRemove->next;
    toRemove->next->prev = List->head;
    free(toRemove);

    // decrement the size of the list
    List->size--;
    return toReturn;
}
