/* mts.c
** Author: Jordan Dennis
** Date: Feb 17th, 2025
**
** This programs objective is to utilize threads in emulating the scheduling of
** multiple threads sharing a common resource.
**
** The program will take in a single argument, a file containing a list of "trains". Each line in the 
** file will represent an individual train. The trains will be given the following format:
** (Direction, Load Time, Cross Time)
**
** Direction: The direction the train is traveling in. This will be either 'e', 'E', 'w', or 'W' where
** 'e' and 'E' represent eastbound trains, with the capitalized letter representing a train with a higher
** priority.
**
** Load Time: The time it takes for the train to load its cargo, starting at time 0.
**
** Cross Time: The time it takes for the train to cross the bridge, starting at the time it finishes loading.
**
** Sources: 
**  - Much of the syntax and overall structure inspired by the "Unix Threads In C" Youtube series by CodeVault
**    - https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2
*/


// include the necessary header files

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h> 
#include <unistd.h> // usleep()
#include <time.h>
#include "queue_thread.h"
#include "train_thread.h"
#include "train_queue.h"
#include "train.h"
#include "utils.h"



#define MAX_TRAINS 100


// Function Prototypes
void* train_thread_routine(void* arg);
char * get_file (int argc, char *argv[]);
void train_thread_creation (int size, struct train **All_Trains_Array, pthread_t th[]);
void train_thread_join (int size, pthread_t th[]);
void* q_thread_routine(void*arg);
void q_thread_join(pthread_t queues[2]);
void q_thread_creation (pthread_t q_threads[]);
void initalize();
void destruction();



// create global mutex
pthread_mutex_t e_head_mutex;
pthread_mutex_t w_head_mutex;
pthread_mutex_t track_mutex;

// create conditions
pthread_cond_t cond_track_busy;
pthread_cond_t cond_east_turn;
pthread_cond_t cond_west_turn;


// create east and west bound train queues
struct train_queue west_bound_q;
struct train_queue east_bound_q;


// global constants to track the number of trains
int total_trains;
int trains_run= 0;
int last_direction = 0;
int consecutive_same_direction = 0;
int east_turn = 0;

struct timespec start_time;

/* ***** MAIN PROGRAM ***** */
int main (int argc, char *argv[])
{

    char *filename;
    filename = get_file(argc, argv);
    //printf("Able to get file name! %s\n", filename);


    // allocate space and initialize variables
    struct tr_array_and_size *arr_and_size;
    struct train **All_Trains_Array = malloc(sizeof(struct train *) * MAX_TRAINS);


    // build the initial list of trains and get how many trains there are
    arr_and_size = build_trains(filename, All_Trains_Array);
    //printf("Successfully built trains!\n");
    
    // store the desired array of trains in All_Trains_Array and the size in size
    All_Trains_Array = arr_and_size->array;
    total_trains = arr_and_size->size;
    free(arr_and_size);


    // train thread creation
    pthread_t th[total_trains];
    // east and west queue thread creation
    pthread_t queues[2];


    clock_gettime(CLOCK_MONOTONIC, &start_time);
    initalize();
    // create the east and west queue threads
    q_thread_creation(queues);
    // iterate over the array of trains, creating a thread for each train.
    train_thread_creation(total_trains, All_Trains_Array, th);
    // ***** DO SOME SYCHRONIZATION OF YOUR THREADS HERE *****



    // where all the trains join again
    sleep(3);
    train_thread_join(total_trains, th);

    q_thread_join(queues);
    // must free memory allocated for trains when finished.
    free_trains(All_Trains_Array, total_trains);
    destruction();

    return 0;
}
/* ***** END OF MAIN FUNCTION ***** */





char * get_file (int argc, char *argv[]){
    // do some error checking to ensure a file is given as an argument
    if (argc != 2)
    {
        printf("Must provide one argument, the file containing the list of trains.\n");
        exit(1);
    }
    
    // Return the command line argument (the filename)
    return argv[1];
}






void initalize(){
    pthread_mutex_init(&e_head_mutex, NULL);
    pthread_mutex_init(&w_head_mutex, NULL);
    pthread_mutex_init(&track_mutex, NULL);
    pthread_cond_init(&cond_east_turn, NULL);
    pthread_cond_init(&cond_west_turn, NULL);
    pthread_cond_init(&cond_track_busy, NULL);
}


void destruction(){
    pthread_mutex_destroy(&e_head_mutex);
    pthread_mutex_destroy(&w_head_mutex);
    pthread_mutex_destroy(&track_mutex);
    pthread_cond_destroy(&cond_east_turn);
    pthread_cond_destroy(&cond_west_turn);
    pthread_cond_destroy(&cond_track_busy);
}