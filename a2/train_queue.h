/* train_queue.h
** Author: Jordan Dennis
** Date February 17th, 2025
**
** Header file to define trains and train queues. 
**
** Train queue will be a doubly linked list, self sorting using the attributes of the adjacent trains, with the new train starting at the head
** and moving towards the back until it reaches a train with a lower priority than itself.
**
**
*/
#ifndef TRAIN_QUEUE_H
#define TRAIN_QUEUE_H
#define _XOPEN_SOURCE 700








/* ***** FUNTION PROTOTYPES ***** */




struct train *place_train(struct train * head, struct train * new);
struct train *run_train(struct train *head);
int new_priority(struct train*head, struct train *new);
void place_here(struct train *head, struct train * new);
struct tr_array_and_size *build_trains(char *, struct train**); 
void free_trains(struct train **array_of_trains, int size);







#endif
// TRAIN_QUEUE_H