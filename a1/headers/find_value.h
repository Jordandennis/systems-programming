/* find_value.h
** Author: Jordan Dennis
** Date: February 12, 2025
**
** This is a header file for the functon find_value, which takes the value to be searched for in a file (such as "cpu cores" or "cpu MHz") **(created with the intentions of being used to in linux file systems)  
** and the path to the file. The function will convatenate the file, passing the output to grep, which will return the line containing the first instance of the value.
**
**
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



#define FIELD_SIZE 300
#define BIG_FIELD 250
#define MAX_LINE_SIZE 40
#define MAX_WORD_SIZE 50
#define MAX_NUM_LINES 1000




char * find_value(char value[], char filepath[]){
/*Given a value and a filepath, open the file in read mode and search for the value. Return the value if found, otherwise return NULL.
**
*/  


    // open pipe and prepare args for child process execution
    char *args0[] = {"cat", filepath, NULL};
    char *args1[] = {"grep", "-m 1", value, NULL};
    int pid1;
    int pid2 = 1;

    // These pipes will be used to send the output to the console from the execution of out child processes
    // directly to the input of a second child process. In this case, concatenate the output of "cat /proc/*filepath" and use
    //grep to search for the value
    int fd1[2];
    int fd2[2];
    if ((pipe(fd1) == -1) || (pipe(fd2) == -1)){
        printf("Error creating pipe\n");
        exit(2);
    }


    // Create fork and run "cat /proc/*filepath" in child process, redirect output to pipe[0]
    pid1 = create_pipe_out_fork(fd1, args0);
    
    // Create fork and run "grep -m 1 value" in child process, reading from pipe[0] and redirecting output to pipe[1]
    pid2 = create_pipe_inout_fork(fd1, fd2, args1);

    // Wait for child processes to finish
    waitpid(pid1, NULL, 0);

    waitpid(pid2, NULL, 0);   


    // allocate some space to read into
    static char output [FIELD_SIZE];
    int rd;
     // Parent process reads from pipe 2
    if ((rd = (read(fd2[0], &output, sizeof(output)))) == -1){
        printf("Error reading from pipe\n");
        exit(3);
    }


    // close pipes
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);


    return output;
}
