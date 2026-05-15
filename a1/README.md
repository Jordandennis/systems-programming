Project: System Processes
Author: Jordan Dennis - V00939664 - UVic
Date: February 8, 2025

This project will take a look at using the fork() operator to create a number of system processes in a linux 
environment, using linux terminal commands to output information about the machine and other OS operations.

In general, this will have two seperate parts: a C program fetch_info.c and a C program pipe4.c.




fetch_info.c - 
Purpose: Print information about the machine/ current processes to the terminal

Arguments: Optional process specification argument (int)

Output: 
With argument - Information on the process with the number of the argument given (ie. fetch_info 3 will give 
information on process number 3).
Without argument - General information on the system, including OS version, CPU, storage in kB and system uptime.




pipe4.c - 
Purpose: Allow the user to pipe up to 4 commands, using the ouptut from the previous as the 
input in the next. 

Usage: The user must provide the full path to the command located (generally) at /usr/bin/*command
where *command is the shell command the user would like to use. 

Example: For example, a user may choose to take the outout of a .txt file, a command for which 
might look like "/usr/bin/cat example.txt" without the parentheses, and then hitting the enter button
to create the first command. The user may choose to do something with that text, such as sort the 
text along by line alphabetically. This command may look something like "/usr/bin/sort" and then pressing 
the enter/return button to add this command to the program. To execute the program, the user would then hit enter
on the empty input line, where the program in this case would then run the first command, and then
using the output of the first command as the input for the second, run the second command.

Output will be the result of the 4 commands, one piped into the next, provided by the user



