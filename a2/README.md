# a2 - Assignment 2 CSC 360
Author: Jordan Dennis
Date: February 17th, 2025  

Assignment 2's purpose is to simulate 

overall idea:

- all trains are created in array
- threads are created and passed a train
- all threads synchronize and begin load time
- as trains are finished loading, they'll add themselves to the appropriate q
- these q threads will wait on conditional mutex "track_busy"
- track_busy will be changed by the train threads, as they should unlock track_busy after their sleep
- 
