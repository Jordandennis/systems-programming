#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "train.h"
#include <ctype.h>
#include <string.h>
#include <unistd.h>




/* **** CREATION AND DESTRUCTION FUNCTIONS ***** */


struct two_int *parse_train(char dir) {
    // helper function for main, determines direction and priority from given character

    struct two_int *out;
    out = (struct two_int*)malloc(sizeof(struct two_int));


    //determine if travelling east or west
    if (tolower(dir) == 'e'){
        out->one = 0;
    }
    else{
        out->one = 1;
    }

    //determine priority
    if ((dir == 'E') || (dir == 'W')){
        out->two = 1;
    }
    else{
        out->two = 0;
    }

    return out;
}




struct train *create_train (int id, int dir, int load, int travel, int prio){
// Create train and dynamically allocate memory for it
// Args: id number, direction, load time, travel time, priority level(one for high, 0 for low)
// Output: pointer to a train.
    struct train *output_train = (struct train *)malloc(sizeof(struct train));

    char direction[8];
    if (dir == 0){
        strncpy (direction, "East", sizeof(direction));
    }
    else{
        strncpy (direction, "West", sizeof(direction));
    }
    output_train-> ID = id;
    output_train-> Dir = dir;
    strncpy(output_train->dir_str, direction, sizeof(output_train->dir_str));
    output_train-> Load = load;
    output_train-> Travel =  travel;
    output_train-> priority = prio;
    output_train-> prev = NULL;
    output_train-> next = NULL;
    output_train-> tail = 1;

    /*
    printf("New train has the following data attached to it: \n \
    id: %d \n \
    directon: %4s \n \
    load time: %d \n \
    travel time: %d \n \
    priority: %d \n", output_train-> ID, output_train-> dir_str, output_train-> Load, output_train-> Travel, output_train-> priority);
    */
    return output_train;
}



void destroy_train(train *tr){
// free the memory allocated during train creation
    printf("destroying train number: %d", tr->ID);
    free(tr);
}

