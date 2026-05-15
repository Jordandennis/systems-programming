#define _XOPEN_SOURCE 700
#include <unistd.h> // usleep()
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h> 
#include <time.h>
#include "utils.h"
#include "train_queue.h"
#include "train.h"
#include "queue_thread.h"


/* ***** TRAIN THREAD FUNCTIONS ***** */
void* train_thread_routine(void* arg){
    // all train threads will start by sleeping the length of their load time, and then adding themselves to the 
    // appropriate queue. They will then signal the appropriate condition variable to let the queue know that a new train is available.


    // sleep, then print that the train is ready to go
    struct train* train_arg = (struct train*)arg;
    usleep(train_arg->Load * 100000);
    char * cur_time = get_time();
    printf("%10s Train %2d is ready to go %4s\n", cur_time, train_arg->ID, train_arg->dir_str);
    free(cur_time);


    // add the train to the appropriate queue
    if (train_arg->Dir == 0) {
        // East bound - lock before modifying queue
        pthread_mutex_lock(&e_head_mutex);
        east_bound_q.q_head = place_train(east_bound_q.q_head, train_arg);
        pthread_mutex_unlock(&e_head_mutex);
        
        // Signal that a new train is available
        pthread_mutex_lock(&track_mutex);
        pthread_cond_signal(&cond_east_turn);
        pthread_mutex_unlock(&track_mutex);
    } else {
        // West bound - lock before modifying queue
        pthread_mutex_lock(&w_head_mutex);
        west_bound_q.q_head = place_train(west_bound_q.q_head, train_arg);
        pthread_mutex_unlock(&w_head_mutex);
        
        // Signal that a new train is available
        pthread_mutex_lock(&track_mutex);
        pthread_cond_signal(&cond_west_turn);
        pthread_mutex_unlock(&track_mutex);
    }
    
    return NULL;
}



void train_thread_creation (int size, struct train **All_Trains_Array, pthread_t th[]){
    // create the threads for the trains
    int i;
    for (i = 0; i < size; i++){
        if (pthread_create(&th[i], NULL, &train_thread_routine, All_Trains_Array[i]) != 0){
            printf("could not create thread!\n");
            exit(1);
        }
        //printf("thread %d has started\n", i);
        // here we'll go through the list of trains and create threads and whatnot
    }

}

void train_thread_join (int size, pthread_t th[]){
    // join the threads for the trains
    for (int j = 0; j < size; j++){
        if(pthread_join(th[j], NULL) != 0){
            printf("could not join thread %d\n", j);
            exit(1);
        }
        //printf("thread %d joined\n", j);
    }
}