#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h> 
#include <unistd.h> // usleep()
#include <time.h>
#include "train_thread.h"
#include "train_queue.h"
#include "train.h"
#include "utils.h"
#include "queue_thread.h"


// Global variable to track whose turn it is
extern int east_turn; // 1 means east's turn, 0 means west's turn

// In mts.c global variables
extern int consecutive_same_direction;  // Track consecutive trains in same direction
extern int last_direction;  // 0=east, 1=west - initialize to west so east goes first

// Function to determine which train has priority
int determine_priority() {
    pthread_mutex_lock(&e_head_mutex);
    pthread_mutex_lock(&w_head_mutex);
    
    struct train* east_train = east_bound_q.q_head;
    struct train* west_train = west_bound_q.q_head;
    
    // Case 1: If either queue is empty
    if (east_train == NULL) {
        pthread_mutex_unlock(&e_head_mutex);
        pthread_mutex_unlock(&w_head_mutex);
        return 0; // Give to west
    }
    
    if (west_train == NULL) {
        pthread_mutex_unlock(&e_head_mutex);
        pthread_mutex_unlock(&w_head_mutex);
        return 1; // Give to east
    }
    
    // Case 2: Anti-starvation rule
    if (consecutive_same_direction >= 1) {
        // If we've had two consecutive trains in the same direction,
        // give priority to the opposite direction
        int result = (last_direction == 0) ? 0 : 1; // opposite of last_direction
        pthread_mutex_unlock(&e_head_mutex);
        pthread_mutex_unlock(&w_head_mutex);
        return result;
    }
    
    // Case 3: Priority comparison
    if (east_train->priority != west_train->priority) {
        // Different priorities - higher priority goes first
        int result = (east_train->priority > west_train->priority) ? 1 : 0;
        pthread_mutex_unlock(&e_head_mutex);
        pthread_mutex_unlock(&w_head_mutex);
        return result;
    }
    
    // Case 4: Same priority, use loading time (ID as proxy)
    if (east_train->ID < west_train->ID) {
        pthread_mutex_unlock(&e_head_mutex);
        pthread_mutex_unlock(&w_head_mutex);
        return 1; // East loaded first
    } else if (west_train->ID < east_train->ID) {
        pthread_mutex_unlock(&e_head_mutex);
        pthread_mutex_unlock(&w_head_mutex);
        return 0; // West loaded first
    } else {
        // Same loading time (shouldn't happen), alternate direction
        int result = (last_direction == 0) ? 0 : 1;
        pthread_mutex_unlock(&e_head_mutex);
        pthread_mutex_unlock(&w_head_mutex);
        return result;
    }
}

/* ***** ROUTINES FOR THE "TRAIN STATIONS" ***** */
void* eastbound_routine(void* arg) {
    struct train_queue* q_arg = (struct train_queue*)arg;
    

    // Loop until all trains have run
    while (trains_run < total_trains) {
        
        // Lock the track mutex to access the shared resource
        pthread_mutex_lock(&track_mutex);
        
        // Wait until it's our turn (east bound turn) or if west is empty and we have trains
        while (!east_turn && 
               !(west_bound_q.q_head == NULL && q_arg->q_head != NULL)) {
            pthread_cond_wait(&cond_east_turn, &track_mutex);
            
            // If all trains have run while we were waiting, exit
            if (trains_run >= total_trains) {
                pthread_mutex_unlock(&track_mutex);
                return NULL;
            }
        }
        
        // Lock the head mutex to access the shared resource
        pthread_mutex_lock(&w_head_mutex);

        // Get the front train from the queue
        struct train* cur_train = q_arg->q_head;
        if (cur_train == NULL) {

            // No trains in our queue, give turn to east
            pthread_mutex_unlock(&w_head_mutex);
            east_turn = 1;
            pthread_cond_signal(&cond_east_turn);
            pthread_mutex_unlock(&track_mutex);
            continue;
        }


        // REMOVE THE CURRENT HEAD FROM THE QUEUE
        q_arg->q_head = cur_train->next;
        if (q_arg->q_head != NULL) {
            q_arg->q_head->prev = NULL;
        }

        // Unlock the head mutex - finished accessing the shared resource
        pthread_mutex_unlock(&w_head_mutex);





        // ***** TRAIN CROSSING *****
        char* cur_time = get_time();
        printf("%10s Train %2d is ON the main track going %s\n", 
               cur_time, cur_train->ID, cur_train->dir_str);
        free(cur_time);


        int time_to_cross = cur_train->Travel;
        
        // Sleep for the travel time (crossing the track)
        // Keep the mutex locked while the train is on the track!
        usleep(time_to_cross * 100000);
        
        // Train finished crossing
        char * now_time = get_time();
        printf("%10s Train %2d is OFF the main track after going %s\n", 
               now_time, cur_train->ID, cur_train->dir_str);
        free(now_time);
        

        // ***** END OF TRAIN CROSSING *****




        
        // Increment the count of trains that have run
        trains_run++;
        
        // Update tracking variables
        if (last_direction == 0) { // If last train was eastbound like this one
            consecutive_same_direction++;
        } else {
            consecutive_same_direction = 1;
            last_direction = 0; // East = 0
        }

        // Determine next turn based on priority rules
        east_turn = determine_priority();
        
        if (east_turn) {
            // Signal that it's now the east queue's turn
            pthread_cond_signal(&cond_east_turn);
            pthread_mutex_unlock(&track_mutex);
        } else {
            // Signal that it's now the west queue's turn
            pthread_cond_signal(&cond_west_turn);
            pthread_mutex_unlock(&track_mutex);
        }
    }
    
    return NULL;
}

void* westbound_routine(void* arg) {

    // GET THE QUEUE ARGUMENT
    struct train_queue* q_arg = (struct train_queue*)arg;
    
    // Loop until all trains have run
    while (trains_run < total_trains) {
        // Lock the track mutex to access the shared resource
        pthread_mutex_lock(&track_mutex);
        
        // Wait until it's our turn (west bound turn) or if east is empty and we have trains
        while (east_turn && 
               !(east_bound_q.q_head == NULL && q_arg->q_head != NULL)) {
            pthread_cond_wait(&cond_west_turn, &track_mutex);
            
            // If all trains have run while we were waiting, exit
            if (trains_run >= total_trains) {
                pthread_mutex_unlock(&track_mutex);
                return NULL;
            }
        }
        

        // ***** OPERATIONS ON CURRENT QUEUE *****
        pthread_mutex_lock(&w_head_mutex);

        // Get the front train from the queue
        struct train* cur_train = q_arg->q_head;

        // Check if there are no trains in the queue
        if (cur_train == NULL) {
            // No trains in our queue, give turn to east
            pthread_mutex_unlock(&w_head_mutex);
            east_turn = 1;
            pthread_cond_signal(&cond_east_turn);
            pthread_mutex_unlock(&track_mutex);
            continue;
        }


        // REMOVE THE CURRENT HEAD FROM THE QUEUE
        q_arg->q_head = cur_train->next;
        if (q_arg->q_head != NULL) {
            q_arg->q_head->prev = NULL;
        }


        pthread_mutex_unlock(&w_head_mutex);


        // ***** END OF OPERATIONS ON CURRENT QUEUE *****



        // Process the train - let it cross the track
        char* cur_time = get_time();
        printf("%10s Train %2d is ON the main track going %s\n", 
               cur_time, cur_train->ID, cur_train->dir_str);
        free(cur_time);

        int time_to_cross = cur_train->Travel;

        
        
        // Sleep for the travel time (crossing the track)
        // Keep the mutex locked while the train is on the track!
        usleep(time_to_cross * 100000);
        
        // Train finished crossing
        char * now_time = get_time();
        printf("%10s Train %2d is OFF the main track after going %s\n", 
               now_time, cur_train->ID, cur_train->dir_str);
        free(now_time);

        
        // Increment the count of trains that have run
        trains_run++;
        
        // Update tracking variables
        if (last_direction == 1) { // If last train was westbound like this one
            consecutive_same_direction++;
        } else {
            consecutive_same_direction = 1;
            last_direction = 1; // West = 1
        }

        // Determine next turn based on priority rules
        east_turn = determine_priority();
        
        if (east_turn) {
            // Signal that it's now the east queue's turn
            pthread_cond_signal(&cond_east_turn);
            pthread_mutex_unlock(&track_mutex);
        } else {
            // Signal that it's now the west queue's turn
            pthread_cond_signal(&cond_west_turn);
            pthread_mutex_unlock(&track_mutex);
        }
    }
    
    return NULL;
}


void q_thread_join(pthread_t queues[2]){
    // Join the east and west queue threads
    int i;
    for (i = 0; i < 2; i++){
        pthread_join(queues[i], NULL);
    }
}


void q_thread_creation (pthread_t q_threads[]){
    // Create the east and west queue threads
    int i;
    struct train_queue *queues[] = {&east_bound_q, &west_bound_q};
    for (i = 0; i < 2; i++){
        if (i == 0){
            if (pthread_create(&q_threads[i], NULL, &eastbound_routine, queues[i]) != 0){
                printf("could not create thread!\n");
                exit(1);
            }
        }
        else{
            if (pthread_create(&q_threads[i], NULL, &westbound_routine, queues[i]) != 0){
                printf("could not create thread!\n");
                exit(1);
            }
        }
    }
}
