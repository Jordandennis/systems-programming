// get_up.h
// Author: Jordan Dennis
// Date: February 12, 2025
// Simple function to get the uptime of the system by executing "uptime -p" and reading the output from the pipe.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

 
#define FIELD_SIZE 300
#define BIG_FIELD 250
#define MAX_LINE_SIZE 40
#define MAX_WORD_SIZE 50
#define MAX_NUM_LINES 1000

char * args[] = {"uptime", "-p", NULL};
int fd1[2];



char * get_up(){
    // open pipe and prepare args for child process execution
    if (pipe(fd1) == -1){
        printf("Error creating pipe\n");
        exit(2);
    }


    // Create fork and run "uptime" in child process, redirect output to pipe[0]
    int pid;
    if ((pid = fork()) == 0){
        // Child process opens
        
        // redirect stdout to pipe
        dup2(fd1[1], STDOUT_FILENO);
        
        close(fd1[1]);
        close(fd1[0]);

        

        //execute "uptime -p"
        execvp(args[0], args);
        printf("Error executing command\n");
        exit(4);
    }

    //
    waitpid(pid, NULL, 0);
 
    static char uptime[FIELD_SIZE];
    int rd;

    // Parent process reads from pipe 2
    if ((rd = (read(fd1[0], &uptime, sizeof(uptime)))) == -1){
        printf("Error reading from pipe\n");
        exit(3);
    }


    // close pipes
    close(fd1[0]);
    close(fd1[1]);



return uptime;
}

