#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "train.h"
#include "train_queue.h"
#include "train_thread.h"
#include "train_queue.h"
#include "train.h"
#include "utils.h"


/* ***** FUNCTION FOR CREATING THE INITIAL ARRAY OF TRAINS ***** */

struct tr_array_and_size *build_trains(char *filename, struct train **array){
    /* Given a filename, open the file for reading and generate an array of pointers to train structs
    ** arguments: filename for opening, existing array of pointers to manipulate
    ** output: a struct containing the number of trains and the array of pointers
    **
    **
    */


    //open a file for reading
    //printf("attempting to build trains!\n");
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        printf("File could not be opened.\n");
        exit(1);
    }


    // allocate memory for the return value.
    struct tr_array_and_size *output = malloc(sizeof(struct tr_array_and_size));


    // initialize train numbering integer.
    int train_number;
    train_number =0;



    // shared variables altered by fscanf(). Safe to do so because concurrency not started yet
    char Dir;
    int Load, Travel;


    // this will look at every line in the given input file and, provided the correct format,
    // will get the relevant information and create a train, adding train to a list of pointers to trains
    while (fscanf(file, " %c %2d %2d", &Dir, &Load, &Travel) == 3){
        // printf("scanf do be looking at the following, home dog:\n %c, %2d, %2d\n", Dir, Load, Travel);

        // determine direction and priority of given train
        int direction, priority;
        struct two_int *dp_tuple;
        dp_tuple = parse_train(Dir);
        direction = dp_tuple->one;
        priority = dp_tuple->two;
        free(dp_tuple);

        // create a pointer to a new train for every line in given file.
        struct train * new_train = create_train(train_number, direction, Load, Travel, priority);

        // add new train to an array of pointers to trains.
        array[train_number] = new_train;

        // train numbers will be 0 based.
        train_number++;
        //printf("Train number now: %d\n", train_number);
    }

    // fill the output fields. I do this because I want the number of trains and the array of trains.
    output->array = array;
    output->size = train_number;
    return output;

}




/* ***** UTILITY FUNCTIONS ***** */

// place_train will take a pointer to a node in the train queue and a new train. It will check to see if the new
// train should be replace the current train in order, and return a pointer to whichever train has higher priority.
struct train *place_train(struct train *head, struct train *new_train) {
    // If queue is empty, new train becomes the head
    if (head == NULL) {
        new_train->next = NULL;
        new_train->prev = NULL;
        return new_train;
    }
    
    // If new train has higher priority than head, insert at beginning
    if (new_train->priority > head->priority) {
        new_train->next = head;
        new_train->prev = NULL;
        head->prev = new_train;
        return new_train;
    }

    // Same priority - if new train loaded faster, it goes first
    if (new_train->priority == head->priority && new_train->Load < head->Load) {
        new_train->next = head;
        new_train->prev = NULL;
        head->prev = new_train;
        return new_train;
    }
    
    // Find the right position to insert based on priority and loading time
    struct train *current = head;
    while (current->next != NULL && 
          (current->next->priority > new_train->priority || 
           (current->next->priority == new_train->priority && 
            current->next->Load <= new_train->Load))) {
        current = current->next;
    }
    
    // Insert after current
    new_train->next = current->next;
    if (current->next != NULL) {
        current->next->prev = new_train;
    }
    current->next = new_train;
    new_train->prev = current;
    
    return head;
}







// Don't return a pointer to an integer
int new_priority(struct train *head, struct train *new) {
    if (head == NULL || new == NULL) {
        return 0;
    }
    
    if (head->Load != new->Load) {
        return (head->Load > new->Load);
    } else {
        return (head->ID > new->ID);
    }
}




void place_here(struct train *head, struct train * new){
    // simple routine to insert a new train
    if (head == NULL || new == NULL){
        return;
    }
    if (head->prev != NULL){
        head->prev->next = new;
    }
    new->prev = head->prev;
    new->next = head;
    head->prev = new;
}










// free all the trains

void free_trains(struct train **array_of_trains, int size){
    // be free little trains...
    for (int i =0; i<size; i++){
        free (array_of_trains[i]);
    }
    
}
