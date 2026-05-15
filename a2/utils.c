#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

extern struct timespec start_time;  // Changed from time_t to struct timespec

char * get_time() {
    char * output = malloc(12 * sizeof(char)); // More space for longer format
    if (output == NULL) {
        fprintf(stderr, "Error: could not allocate memory for time string\n");
        exit(EXIT_FAILURE);
    }
    
    struct timespec current;
    clock_gettime(CLOCK_MONOTONIC, &current);
    
    // get the total elapsed time
    double elapsed = (current.tv_sec - start_time.tv_sec) + 
                    (current.tv_nsec - start_time.tv_nsec) / 1.0e9;
    
    // convert elapsed time to hours, minutes, seconds, and tenths
    int hours = (int)(elapsed / 3600);
    int minutes = (int)((elapsed - hours * 3600) / 60);
    int seconds = (int)(elapsed - hours * 3600 - minutes * 60);
    int tenths = (int)((elapsed - (int)elapsed) * 10);
    
    sprintf(output, "%02d:%02d:%02d.%01d", hours, minutes, seconds, tenths);
    return output;
}