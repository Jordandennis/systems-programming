/* pipe4.c
** Author: Jordan Dennis
** Date: February 8th, 2025
**
** The following program is designed to allow a user to pipe up to 4 shell commands together,
** using the output of a command as the input of the next. The user can run the program using the 
** command ./pipe4. After running the command, the user will be prompted to input the command
** they would like to use. The user MUST provide the full path to the shell command they would like to use, 
** generally located at /usr/bin/command* where command* is the shell command they would like to use. The user
** can also provide any arguments to the shell command that are supported by the command by seperating the 
** argument from the command using a space (ex. usr/bin/ls -a OR usr/bin/cat example.txt -1)
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
#include "pipe_fork.h"
#include "find_value.h"

int main(){
    /* This functions job should be to create the child processes necessary for all 
    ** commands given by the user, and to error check the creation/completion of the processes.
    */


 
    return 0;
}
