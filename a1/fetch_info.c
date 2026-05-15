/* fetch_info.c
** Author: Jordan Dennis
** Date: February 8th, 2025
** 
** fetch_info.c is a C program designed to be run on a linux operating system from the command line with the following
** command: "./fetch_info *x" where *x is an optional argument (int) which correlates with the process number you would like 
** information about.
**
** When passed an argument, the function will output information about the process corresponding
** with the given argument. For example, using the command "./fetch_info 4" will give information on process 4, which
** can generally be found at /proc/4 on a linux operating system. 
**
** When the program is called without an argument, the program will return general information about the
** system it is being run on. For example, the command "./fetch_info" will return information about the CPU model,
** uptime, storage and other things related to the machine.
** 
**
** Sources:
**  *Introduction to FIFOs (aka named pipes) in C - Code Vault - 
**  https://www.youtube.com/watch?v=2hba3etpoJg&list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY&index=8
** 
** *(The entire youtube series was a great resource for completing this project.)
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
#include "./headers/pipe_fork.h"
#include "./headers/find_value.h"
#include "./headers/get_up.h"

 
#define FIELD_SIZE 300
#define BIG_FIELD 250
#define MAX_LINE_SIZE 40
#define MAX_WORD_SIZE 50
#define MAX_NUM_LINES 1000

// Structs for process and system information
struct process_info {
    int pid;
    char name[FIELD_SIZE];
    char filename[FIELD_SIZE];
    char threads[FIELD_SIZE];
    char context_switches[FIELD_SIZE];
};

struct system_info {
    char cpu_model[FIELD_SIZE];
    char cpu_cores[FIELD_SIZE];
    char OS[FIELD_SIZE];
    char uptime[FIELD_SIZE];
    char memory[FIELD_SIZE];
};

// Function prototypes
struct process_info get_process(char *process_id);
struct system_info get_system();
void print_proc_info (struct process_info info);
void print_sys_info (struct system_info info);



int main (int argc, char *argv[]) {
/* Main function for the fetch_info program. This program is used to fetch information about a given process ID or system. 
** The program takes a process ID number as an argument and prints out the following information about the process: 
** 1. Process ID Number
** 2. Name of the executable
** 3. Filename of the executable
** 4. Threads
** 5. Total Context Switches
*/

    if (argc == 1) {
        // If no arguments are given, print out system information
        struct system_info info;
        info = get_system();
        print_sys_info(info);



    } else if (argc == 2) {
        // If an argument is given, print out process information
        struct process_info info;
        info = get_process(argv[1]);
        print_proc_info(info);
    }
    else{
        printf("Invalid number of arguments\n");
        exit(1);
    }


    return 0;
}





struct process_info get_process(char *process_id) {
/* The program takes a process ID number as an argument and prints out the following information about the process: 
** 1. Process ID Number
** 2. Name of the executable
** 3. Filename of the executable
** 4. Threads
** 5. Total Context Switches
*/
    struct process_info output;
    char filepath[16];
    filepath[0] = '\0';
    strncat( "/proc/", filepath, sizeof(char));
    strncat(process_id , filepath, sizeof(char));
    strncat("/status", filepath, sizeof(char));
    strncpy(output.name, find_value("Name", filepath), FIELD_SIZE);
    strncpy(output.filename, find_value("Name", filepath), FIELD_SIZE);
    strncpy(output.threads, find_value("Threads", filepath), FIELD_SIZE);
    strncpy(output.context_switches, find_value("voluntary_ctxt_switches", filepath), FIELD_SIZE);

    return output;
}






struct system_info get_system() {
/* Open a number of files in /proc in read mode, parse the information about the system and return the information as a struct.
** Argumens: None
** Returns: struct system_info
*/

    struct system_info output;
    
    char *mname[FIELD_SIZE];
    char mcores[FIELD_SIZE];
    char mOS[FIELD_SIZE];
    char muptime[FIELD_SIZE];
    char mmemory[FIELD_SIZE];
    &mname = find_value("model name","/proc/cpuinfo");
    strncpy(output.cpu_model, find_value("model name","/proc/cpuinfo"), FIELD_SIZE);
    strncpy(output.cpu_cores, find_value("cpu cores", "/proc/cpuinfo") , FIELD_SIZE);
    strncpy(output.OS, find_value("Linux version", "/proc/version"), FIELD_SIZE);
    strncpy(output.memory, find_value("MemTotal", "/proc/meminfo" ), FIELD_SIZE);
    strncpy(output.uptime, get_up(), FIELD_SIZE);


    return output;

}

void print_proc_info (struct process_info info) {
/* Print out the information about a process in a readable format.
** Arguments: struct process_info
** Returns: None
*/
    printf("print proc");
    printf("Process ID: %d\n", info.pid);
    printf("Name: %s\n", info.name);
    printf("Filename: %s\n", info.filename);
    printf("Threads: %s\n", info.threads);
    printf("Context Switches: %s\n", info.context_switches);
}


void print_sys_info (struct system_info info) {
/* Print out the information about the system in a readable format.         
** Arguments: struct system_info
** Returns: None
*/
    printf("print sys\n");
    printf("CPU Model: %s", info.cpu_model);
    printf("CPU Cores: %s", info.cpu_cores);
    printf("OS: %s", info.OS);
    printf("Memory: %s", info.memory);
    printf("Uptime: %s", info.uptime);
}
