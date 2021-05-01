# CS441/541 Project 5

## Author(s):

Shyue Shi Leong and Ze Jia Lim


## Date:

11/26/2020


## Description:

This is an implementation of a program that creates the requisite number of ninja and pirate threads, and use semaphores to synchronize the 
costume department.


## How to build the software

 1. Navigate into the cs441-synchro directory.
 2. Type "make" on the command line to compile the code.


## How to use the software

 1. Make sure you are in the cs441-synchro directory before using the software.
 2. You need to type the required arguments on the command line which is "./costume 10". The 10 in the example is a required argument that
    indicates how many seconds will the program run before terminating. You can add up to 3 optional arguments follwoing the required arguments.
	The first optional argument is the number of pirates running in the program. It should be a positive integer greater than 0 and the default 
	number of pirate is 5. The number of ninjas running in the program is the second optional argument. It should also be a positive integer
	greater than 0 and the default is 5 as well. The last optional argument is the number of booth available for costume preparation. It should
	be a positive integer greater than 0 and the default is 2. 


## How the software was tested

The software was tested with multiple combinations of command line arguments. Since this is a synchronization project, we could not just fully rely on
the final statistics of the each thread to check for the corroctness of the program. Hence, we went through the output of each threads to make sure 
that they are working as intended for each combinations that we tested. After that, then we discussed about the final statistics for each thread to see
whether they make sense or not. We also ran each combinations for multiple times to make sure that there is no deadlocks (or a very low chance). Some of 
the combination includes when the number of booth exceeds the number of ninjas and/or pirates, the number of booth is smaller than the number of pirate 
and/or ninjas, the number of booth equals to the number of pirate and/or ninja.

Test Cases that we have done:
1. The number of booth exceeds the number of ninjas and/or pirates:
   10 3 2, 10 2 3, 10 4 4, 10 3 3
2. The number of booth is smaller than or equal to the number of pirates and/or ninjas:
   10 1 1 1, 10 2 2, 10 1 2 5, 10 2 1 5, 10 1 10 11, 10 5 10 11, 10 10 1 11, 10 10 5 11
3. To ensure fairness, avoid starvation and ensure correct utilization of the queues:
   10 4 4 1
4. To ensure the program can execute with different time to live:
   1 3 2, 5 3 2, 15 3 2
5. Larger values of pirates/ninjas/booths
   10 50 50 10, 10 10 10 6969

## Known bugs and problem areas

None after some thorough testing.

## Special Section
1. We have two methods each describing the behavior of the pirates and ninjas respectively. Both methods are executing similarly but the parameters used 
   in some method calls are different. When printing a statement to indicate the thread have change their state, we used a binary mutex to make sure
   that only one thread can print the statement at a time, which helps us better when debugging our code. 
   
   Before the threads enter the costume department (critical section), we will start by having a do while loop containing the implementation of what each 
   pirate thread will do and it will terminate once the time is up. For each pirate thread, they will start by printing a statement showing that they are 
   waiting for their turn to enter the costume department. After that, we will hold the pirate threads in a semaphore if it is not their turn to enter the 
   costume department. Here, there is a mutex with a value of 1 to ensure that only one thread can enter this section at a time. In this section, we will 
   check if the number of pirates exceed the number of booth or not. If the number of pirate is lower than the number of booth, then we do not need to 
   sleep any of the pirate threads before they enter the costume department. Otherwise, we have split it into two sections, the first section is the 
   implementation when the pirate threads entered the first time and the second implementation is when the threads have entered more than once. When the 
   threads are entering the costume department for the first time, we will allow up to number of booth of pirates to go through, adding the remaining threads 
   in a queue and hold them in a semaphore. When the threads are entering the costume department more than once, then we will add every thread into queue and 
   hold them in a semaphore except for the last thread to arrived. The last thread will be added to the queue and hold by a semaphore as well, but before that 
   it will wake up to the number of booth amount of pirate threads for them to enter the costume department. One more thing we did was to allow the threads to 
   release the resources held by the threads before allowing them to be held by the semaphore. 
   
   In the costume department (critical section), it is protected by a mutex with a value of the number of booth in the costume department to ensure the 
   number of threads that entered the costume department does not exceed the number of booth at a time. The threads that entered the costume department, 
   they will first increment the number of times they entered the costume department by 1 and print a statement indicating that the thread is in the costume 
   department doing costume preparation. Before exiting the costume department, the threads will be slept for a random amount of microseconds between 0 to 
   50000 to simulate that the threads are doing costume preparation. 
   
   When the threads are exiting the costume department (critical section), it is protected with a mutex with a value of 1 to make sure that there is only one
   thread can execute this section at a time. For each threads that enter this section, they will increment the number of times they left the costume department 
   by 1 and print a statement indicating they have left the costume department. After that, they will change the variable indicating who is supposed to enter the 
   costume department next, update other global variables and the last thread to exit this section will have another thing to do. For the last thread, it 
   will need to wake up the ninja threads that are block by the semaphore when they have arrived. The ninja will be blocked if the pirates managed to entered the
   costume department, which means we need to wake the ninja threads up and it is their turn to enter the costume department. After that, the pirate threads will 
   be slept for a random amount of microseconds between 0 and 1000000 to simulate that they leave the costume department to do their thing before returning to the 
   costume department again. 
   
   The above explanations only describes the behavior of the pirate threads, the ninja threads will have similar implementation but the parameters used for certain 
   method calls will be different. For example, the ninja threads need to wake the pirate threads up after the ninja threads have left the costume department. Some 
   of the global variables are unique for pirate threads and ninja threads. Despite that, the behavior of the pirate and ninja threads are the same. 
   
2. a) Our solution avoid deadlock by making sure that there is always at least one thread running when it is their turn to run. The last thread that arrived will wake
      the appropriate number of threads for them to enter the critical section. During the thread's first execution, we only sleep the threads when it exceed the 
      number of booth. Furthermore, the threads that left the critical section will signal the other faction that the booth is available for them to use and wake 
      them up for them to continue their execution. There is one more implementation that our solution does to avoid deadlock is when the time is up and there are still
      some threads that are in the queue blocked by semaphore. Our solution make sures that all threads are able to terminate execution when the time is up by waking them
      up and then exit each of the thread instantly.
      
   b) Our solution does prevent pirates and ninjas from being in the costume department at the same time. We have one global variable to indicate whose turn it is to 
      enter the costume department. For example, when it is pirate's turn to enter the costume department, the ninja threads will be blocked by a semaphore after 
      they left the costume department and printed a statement showing that they are waiting. When pirates exit the costume department, the global variable will be 
      changed and indicate that it is ninja's turn and pirates will wake the ninjas up from the semaphore. This implementation is done for both pirates and ninjas to
      prevent from being able to be in the costume department at the same time. 
      
   c) We have a global variable to indicate whose turn it is to enter the costume department. When it is not their turn to enter the costume department, they will be 
      blocked by a semaphore until it is their turn to enter the costume department. When pirate threads entered and left the costume department, the global 
      variable will change to be ninjas turn to enter the costume department and vice versa. This will make sure that pirate and ninja threads have their chance to 
      enter the costume department. Therefore, this is how our solution avoid depriving one side or the other access to the costume department and how our solution is "fair".
      
   d) Our solution does prevent "starvation" because we have implemented two linked list that will act as FIFO queue, one for pirates and one for ninjas. Each thread also 
      have their own semaphore to block them whenever they should be blocked. When a thread is unable to enter the critical section because of insufficient number of booth, 
      then their semaphores are added in to the end of the FIFO queue and they are blocked by their own semaphore to prevent excess ninja of pirate thread to enter the 
      critical section. When deciding which thread to enter the critical section, the program will always remove number of booth of threads from the front of the FIFO queue 
      if the queue is not empty to prevent them from waiting indefinitely in the queue. 
   
   e) The linked lists that we have implemented will act as FIFO queue and we have one for each faction. We will add the item to the end of the list when adding an item into 
      the queue and we will remove the first item in the list when removing an item in the queue. This make sures that which pirate or ninjas to be added into their 
      respectively queue first will be the first one to be remove from the queue. This is how our solution managed to preserve the order of pirates and ninjas waiting on the 
      costume department.
   
   f) Our solution does keep the costume booth relatively full because the threads will signal the other faction to continue execution when the threads completed the costume 
      preparation in the costume department. Hence, there will always be someone in the booths during the program execution. Our solution also have good resource utilization 
      because we did not sleep the threads at all when the number of pirates and ninjas are lower or equal to the number of costume booth. Furthermore, we make sure that the 
      threads will wake up to enter the costume department (critical section) when the last thread have arrived and waiting to enter the costume department (critical section).
      Our program would not hold any unneccessary resources at any given point in time. 
      
3. We managed to verified that our implementation macthes our design by observing the tabulated results printed when the program terminates. We make sure that the program does 
   not have deadlock when running and make sure that there is no thread that have an extremely low or high count of entering or leaving the costume department. Furthermore, we 
   also verified our implementation by observing the printing statement printed by individual threads. For example, the ninja threads should be waiting when pirate threads are 
   entering the costume booth and the ninja threads should enter the moment pirates are done.

4. To ensure the correctness of the implementation, we have done some thorough testing on the code. The first testing that we did was checking if the program can handle 
   different number of command line arguments. Some examples we tried was "./costume", "./costume -1", "./costume 10 -1" and so on. The second testing that we did was trying
   different number of pirates and ninjas that does not exceed the number of booth. Some examples are "./costume 10 1 2 5" and "./costume 10 2 1 5". The thrid patch of testing 
   was number of pirates and ninjas that are equal to the number of booth. After that we move on to testing using number of ninajs and pirates that exceed the number of booths.
   The last testing that we did was trying out different corner cases and extreme cases that we can think of. Some of the testing that we did include "./costume 10 4 4 1" to 
   show that we utilize the queue correctly, "./costume 10 20 20 10" and "./costume 10 5 5 100"to show that our solution can handle number of pirates, ninjas and booths that 
   are big.