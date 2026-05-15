/* pipe_fork.h
** Author: Jordan Dennis    
** Date: February 12, 2025
**
** The following functions serve to create forks of an existing process, executing a command which will either recieve input from the standard input, send output to the standard output, or both.
**
** Arguments: file descriptor (int[2]), **optional** second file descriptor (int[2]), vector of arguments to be executed using execvp (char *[])
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


int create_pipe_out_fork(int fd1[2], char *args[]){
    int pid;
    if ((pid = fork()) == 0){
        // Child process opens
        
        // redirect stdout to pipe
        dup2(fd1[1], STDOUT_FILENO);
        
        close(fd1[1]);
        close(fd1[0]);

        

        //execute "cat /proc/*filepath"
        execvp(args[0], args);
        printf("Error executing command\n");
        exit(4);
    }
return pid;
}


 
int create_pipe_in_fork(int fd1[2], char *args[]){
    int pid;
    if ((pid = fork()) == 0){
        // Child process opens
        
        // redirect stdout to pipe
        dup2(fd1[0], STDIN_FILENO);
        
        close(fd1[1]);
        close(fd1[0]);
        

        //execute "grep -m 1 value"
        execvp(args[0], args);
        printf("Error executing command\n");
        exit(4);
    }
    return pid;
}



int create_pipe_inout_fork(int fd1[2], int fd2[2], char *args[]){

    int pid;
    if ((pid = fork()) == 0){
        // Child process opens
        pid = 1;
        
        // redirect input to pipe and output to pipe2
        dup2(fd1[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);
        
        close(fd1[1]);
        close(fd1[0]);
        close(fd2[1]);
        close(fd2[0]);
        

        //execute "grep -m 1 value"
        execvp(args[0], args);
        printf("Error executing command\n");
        exit(4);
    }
    return pid;
}