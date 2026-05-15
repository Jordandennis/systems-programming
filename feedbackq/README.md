# MLFQ Implementation

**Author:** Jordan Dennis
**Date:** March 21, 2025

## Necessary Files
- feedbackq.c
- queue.c
- queue.h
- makefile

Create executable "schedule" with "make schedule" at the command line

Execute program with 
```bash
./schedule "input.txt"
```
where "input.txt" is the name of a text file, formatted to be a task schedule as described at `https://gitlab.csc.uvic.ca/courses/2025011/CSC360_COSI/course_code/course_notes/-/tree/master/Assignments/Assignment-3?ref_type=heads` (This link mayy deprecate at any time, I am not the maintainer of it).

Output will be a round robin scheduling of the task schedule outlined in the text file passed as input.
