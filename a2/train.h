/* train.h 
** Author: Jordan Dennis
** Date: Feb 28th, 2025
** 
** Implementation of the train struct and some necessary functions for creating and destroying trains.
**
*/



#ifndef TRAIN_H
#define TRAIN_H

#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>




// necessary for returning two integers from function parse_train
struct two_int{
int one;
int two;
};


// necessary for returning an array of pointers to trains and the size of the array
struct tr_array_and_size{
    struct train ** array;
    int size;
};



// definition of train struct
typedef struct train {
    int ID;
    int Dir;
    char dir_str[8];
    int Load;
    int Travel;
    int priority;
    struct train* prev;
    struct train* next;
    int tail;
}train;


// definition of train queue struct
typedef struct train_queue{
    struct train *q_head;
}train_queue;


struct two_int *parse_train(char dir);
struct train *create_train (int, int, int, int, int);
void destroy_train(struct train* tr);



#endif // TRAIN_H