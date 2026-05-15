/* queue_thread.h
** Author: Jordan Dennis
** Date: Feb 28th, 2025
** 
** Header file to define the queue threads and the necessary functions to create and join them.
**
**
*/

#define _XOPEN_SOURCE 700
#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H




extern clock_t time_elapsed;

// global mutex
extern pthread_mutex_t e_head_mutex;
extern pthread_mutex_t w_head_mutex;
extern pthread_mutex_t track_mutex;

// conditions
extern pthread_cond_t cond_track_busy;
extern pthread_cond_t cond_east_turn;
extern pthread_cond_t cond_west_turn;


// create east and west bound train queues
extern struct train_queue west_bound_q;
extern struct train_queue east_bound_q;


// global constants to track the number of trains
extern int east_turn;
extern int trains_run;
extern int total_trains;


void* eastbound_routine(void*arg);
void* westbound_routine(void*arg);
void q_thread_join(pthread_t queues[2]);
void q_thread_creation (pthread_t q_threads[]);



#endif // QUEUE_THREAD_H