/*
 * Some starter code for CSC 360, Spring 2025, Assignment #4
 *
 * Prepared by: 
 * Michael Zastre (University of Victoria) -- 2024
 * 
 * Modified for ease-of-use and marking by 
 * Konrad Jasman (University of Victoria) -- 2025
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <unistd.h>
 
 /*
  * Some compile-time constants.
  */
 
 #define REPLACE_NONE 0
 #define REPLACE_FIFO 1
 #define REPLACE_LRU  2
 #define REPLACE_CLOCK 3
 #define REPLACE_OPTIMAL 4
 
 
 #define TRUE 1
 #define FALSE 0
 #define PROGRESS_BAR_WIDTH 60
 #define MAX_LINE_LEN 100
 
 
 /*
  * Some function prototypes to keep the compiler happy.
  */
 int setup(void);
 int teardown(void);
 int output_report(void);
 void add_to_fifo_queue(int);
 long int select_victim();
 long resolve_address(long, int);
 void error_resolve_address(long, int);
 
 
 /*
  * Variables used to keep track of the number of memory-system events
  * that are simulated.
  */
 int page_faults = 0;
 int mem_refs    = 0;
 int swap_outs   = 0;
 int swap_ins    = 0;
 
 
 /*
  * Page-table information. You are permitted to modify this in order to
  * implement schemes such as CLOCK. However, you are not required
  * to do so.
  */
 struct page_table_entry *page_table = NULL;
 struct page_table_entry {
     long page_num;
     int dirty;
     int free;
     unsigned long last_used;           // Added for LRU implementation
     int referenced;                    // Added for CLOCK replacement scheme
 };
 long use_number = 0; // Added for LRU tracking

 long int *fifo_queue = NULL;  // Array to hold frame numbers in order of loading
 long int fifo_head = 0;       // Index of oldest frame in the queue
 long int fifo_tail = 0;       // Index where next frame will be inserted
 long int fifo_count = 0;      // Number of frames currently in the queue

 long int clock_hand = 0;      // Keep track of positioning of clock hand



 
 
 /*
  * These global variables will be set in the main() function. The default
  * values here are non-sensical, but it is safer to zero out a variable
  * rather than trust to random data that might be stored in it -- this
  * helps with debugging (i.e., eliminates a possible source of randomness
  * in misbehaving programs).
  */
 
 int size_of_frame = 0;  /* power of 2 */
 int size_of_memory = 0; /* number of frames */
 int page_replacement_scheme = REPLACE_NONE;
 

 /*
  * Function to choose the frame which should be removed to 
  * accomodate a new frame. Will use the global variable 
  * page_replacement_scheme in order to select.
  */

long int select_victim(){
    long int victim_frame = -1;
    int i;
    
    switch(page_replacement_scheme) {
        // if FIFO, get page queue head and update queue
        case REPLACE_FIFO:
            // Check if queue has been initialized
            if (fifo_queue == NULL || fifo_count == 0){
                // If queue is empty/not initialized, default to first frame
                victim_frame = 0;
                printf("FIFO queue not initialized, using frame 0\n");
            } 
            else {
                // Get the oldest frame from the queue (the head)
                victim_frame = fifo_queue[fifo_head];
                
                // Update head pointer for next time (circular)
                fifo_head = (fifo_head + 1) % size_of_memory;
                fifo_count--;
            }
            break;



            
        case REPLACE_LRU:
            // Find frame with smallest (oldest) last_used value
            victim_frame = 0;
            for (i = 1; i < size_of_memory; i++) {
                if (page_table[i].last_used < page_table[victim_frame].last_used) {
                    victim_frame = i;
                }
            }
            break;
            
        case REPLACE_CLOCK:
            
            // Keep moving clock hand until finding a non-referenced page
            while (1) {
                // If page is not referenced, select it as victim
                if (page_table[clock_hand].referenced == 0) {
                    victim_frame = clock_hand;
                    clock_hand = (clock_hand + 1) % size_of_memory;
                    break;
                } else {
                    // Give second chance - clear referenced bit and move on
                    page_table[clock_hand].referenced = 0;
                    clock_hand = (clock_hand + 1) % size_of_memory;
                }
            }
            break;
            
        case REPLACE_OPTIMAL:
            // Will need to implement logic that looks at future references
            // This requires preprocessing the memory trace
            // Placeholder for optimal implementation
            break;
            
        case REPLACE_NONE:
        default:
            // No replacement scheme specified or unknown scheme
            victim_frame = -1;
            break;
    }
    
    return victim_frame;
}


 // Add this function before resolve_address()
 void add_to_fifo_queue(int frame) {
     // Only track for FIFO replacement
     if (page_replacement_scheme != REPLACE_FIFO || fifo_queue == NULL) {
         return;
     }
     
     // Add the frame to the tail of the queue
     fifo_queue[fifo_tail] = frame;
     fifo_tail = (fifo_tail + 1) % size_of_memory;
     
     // Ensure we don't exceed queue capacity
     if (fifo_count < size_of_memory) {
         fifo_count++;
     }
 }
 
 /*
  * Function to convert a logical address into its corresponding 
  * physical address. The value returned by this function is the
  * physical address (or -1 if no physical address can exist for
  * the logical address given the current page-allocation state.
  */
 
 long resolve_address(long logical, int memwrite){
     int i;
     long page, frame;
     long offset;
     long mask = 0;
     long effective;
 
     /* Get the page and offset */
     page = (logical >> size_of_frame);
 
     for (i=0; i<size_of_frame; i++){
         mask = mask << 1;
         mask |= 1;
     }
     offset = logical & mask;
 
     /* Find page in the inverted page table. */
     frame = -1;
     for ( i = 0; i < size_of_memory; i++ ){
         if (!page_table[i].free && page_table[i].page_num == page){

             // don't think it really matters whether or not the page is dirty
             if (memwrite){
                page_table[i].dirty = 1;
             }
             page_table[i].referenced = 1;              // Update for CLOCK implementation
             page_table[i].last_used = use_number++;    // Update for LRU implementation
             frame = i;
             break;
         }
     }
 
     /* If frame is not -1, then we can successfully resolve the
      * address and return the result. */
     if (frame != -1){
         effective = (frame << size_of_frame) | offset;
         return effective;
     }
 
 
     /* If we reach this point, there was a page fault. Find
      * a free frame. */
     page_faults++;
 
     for ( i = 0; i < size_of_memory; i++){
         if (page_table[i].free){
             frame = i;
             page_table[i].free = 0;
             page_table[i].last_used = use_number++;
             if (memwrite){
                page_table[i].dirty = 1;
             }
             page_table[i].page_num = page;
             break;
         }
     }
 
     /* If we found a free frame, then patch up the
      * page table entry and compute the effective
      * address. Otherwise return -1.
      */
     if (frame != -1){
         page_table[frame].page_num = page;
         page_table[frame].free = FALSE;
         add_to_fifo_queue(frame);
         swap_ins++;
         effective = (frame << size_of_frame) | offset;
         return effective;
     } else {
         frame = select_victim();
         page_table[frame].page_num = page;
         add_to_fifo_queue(frame);

         // Keep track of swap outs if page written to previously
         if (!page_table[frame].free && page_table[frame].dirty){
            swap_outs++;
         }

         // Track written to status for swap_outs
         if (memwrite){
            page_table[frame].dirty = 1;
         }
         
         page_table[frame].referenced = 1;              // Track ref status for CLOCK
         page_table[frame].last_used = use_number++;    // Track use number for LRU
         swap_ins++;

         effective = (frame << size_of_frame) | offset;
         return effective;
     }
 }
 
 
 
 /*
  * Super-simple progress bar.
  */
 void display_progress(int percent){
     int to_date = PROGRESS_BAR_WIDTH * percent / 100;
     static int last_to_date = 0;
     int i;
 
     if (last_to_date < to_date){
         last_to_date = to_date;
     } else {
         return;
     }
 
     printf("Progress [");
     for (i=0; i<to_date; i++){
         printf(".");
     }
     for (; i<PROGRESS_BAR_WIDTH; i++){
         printf(" ");
     }
     printf("] %3d%%", percent);
     printf("\r");
     fflush(stdout);
 }
 
 
 // Line 376 in main
 int setup(){
     int i;
 
     page_table = (struct page_table_entry *)malloc(
         sizeof(struct page_table_entry) * size_of_memory
     );

     fifo_queue = (long int *)malloc(sizeof(long int) * size_of_memory);   

    
     if (page_table == NULL){
         fprintf(stderr,
             "Simulator error: cannot allocate memory for page table.\n");
         exit(1);
     }

     if (fifo_queue == NULL){
         fprintf(stderr,
             "Simulator error: cannot allocate memory for page queue.\n");
         exit(1);
     }
     fifo_head = 0;
     fifo_count = 0;
     fifo_tail = 0;

     for (i=0; i<size_of_memory; i++){
         page_table[i].free = TRUE;
         page_table[i].referenced = 0; // Initialize referenced bit for CLOCK
         page_table[i].last_used = 0;  // Initialize last use time for LRU
     }
 
     return -1;
 }
 
 
 int teardown(){
     free (page_table);
     free (fifo_queue);
     return -1;
 }
 
 
 void error_resolve_address(long a, int l){
     fprintf(stderr, "\n");
     fprintf(stderr, 
         "Simulator error: cannot resolve address 0x%lx at line %d\n",
         a, l
     );
     exit(1);
 }
 
 
 int output_report(){
     printf("\n");
     printf("Memory references: %d\n", mem_refs);
     printf("Page faults: %d\n", page_faults);
     printf("Swap ins: %d\n", swap_ins);
     printf("Swap outs: %d\n", swap_outs);
 
     return -1;
 }
 
 
 int main(int argc, char **argv){
     /* For working with command-line arguments. */
     int i;
     char *s;
 
     /* For working with input file. */
     FILE *infile = NULL;
     char *infile_name = NULL;
     struct stat infile_stat;
     int  line_num = 0;
     int infile_size = 0;
 
     /* For processing each individual line in the input file. */
     char buffer[MAX_LINE_LEN];
     long addr;
     char addr_type;
     int  is_write;
 
     /* For making visible the work being done by the simulator. */
     int show_progress = FALSE;
 
     /* Process the command-line parameters. Note that the
      * REPLACE_OPTIMAL scheme is not required.
      */
     for (i=1; i < argc; i++){
        // This will determine the page replacement scheme
         if (strncmp(argv[i], "--replace=", 9) == 0){
             s = strstr(argv[i], "=") + 1;
             if (strcmp(s, "fifo") == 0){
                 page_replacement_scheme = REPLACE_FIFO;
             } else if (strcmp(s, "lru") == 0){
                 page_replacement_scheme = REPLACE_LRU;
             } else if (strcmp(s, "clock") == 0){
                 page_replacement_scheme = REPLACE_CLOCK;
             } else if (strcmp(s, "optimal") == 0){
                 page_replacement_scheme = REPLACE_OPTIMAL;
             } else {
                 page_replacement_scheme = REPLACE_NONE;
             }

         // This will fill the rest of the necessary parameters from the command line
         } else if (strncmp(argv[i], "--file=", 7) == 0){
             infile_name = strstr(argv[i], "=") + 1;
         } else if (strncmp(argv[i], "--framesize=", 12) == 0){
             s = strstr(argv[i], "=") + 1;
             size_of_frame = atoi(s);
         } else if (strncmp(argv[i], "--numframes=", 12) == 0){
             s = strstr(argv[i], "=") + 1;
             size_of_memory = atoi(s);
         } else if (strcmp(argv[i], "--progress") == 0){
             show_progress = TRUE;
         }
     }
    
     if (infile_name == NULL){
         infile = stdin;
     } else if (stat(infile_name, &infile_stat) == 0){
         infile_size = (int)(infile_stat.st_size);
         /* If this fails, infile will be null */
         infile = fopen(infile_name, "r");  
     }
 
     // Make sure we have all the information we need.
     if (page_replacement_scheme == REPLACE_NONE ||
         size_of_frame <= 0 ||
         size_of_memory <= 0 ||
         infile == NULL)
     {
         fprintf(stderr, 
             "usage: %s --framesize=<m> --numframes=<n>", argv[0]);
         fprintf(stderr, 
             " --replace={fifo|lru|optimal} [--file=<filename>]\n");
         exit(1);
     }
 
 

 // WHERE THE FUNCTION ACTUALLY DOES STUFF
     setup(); // line 248
 
     while (fgets(buffer, MAX_LINE_LEN-1, infile)){
         line_num++;
         if (strstr(buffer, ":")){
             sscanf(buffer, "%c: %lx", &addr_type, &addr);
             if (addr_type == 'W'){
                 is_write = TRUE;
             } else {
                 is_write = FALSE;
             }
 
             if (resolve_address(addr, is_write) == -1){
                 error_resolve_address(addr, line_num);
             }
             mem_refs++;
         } 
 
         if (show_progress){
             display_progress(ftell(infile) * 100 / infile_size);
         }
     }
     
 
     teardown();
     output_report();
 
     fclose(infile);
 
     exit(0);
 } 