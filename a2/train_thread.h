/* train_thread.h
** Author: Jordan Dennis
** Date: Feb 28th, 2025
** 
** Header file to define the train thread functions.
** 
**
*/



#ifndef TRAIN_THREAD_H
#define TRAIN_THREAD_H




extern clock_t time_elapsed;

extern struct train_queue west_bound_q;
extern struct train_queue east_bound_q;

extern clock_t time_elapsed;

struct train;


void* train_thread_routine(void* arg);
void train_thread_creation (int size, struct train **All_Trains_Array, pthread_t th[]);
void train_thread_join (int size, pthread_t th[]);




#endif // TRAIN_THREAD_H