# Threads
**Author:** Jordan Dennis
**Date:** February 17th, 2025  

---

## Overall Idea
- all trains are created in array
- threads are created and passed a train
- all threads synchronize and begin load time
- as trains are finished loading, they'll add themselves to the appropriate q
- these q threads will wait on conditional mutex "track_busy"
- track_busy will be changed by the train threads, as they should unlock track_busy after their sleep

## Note
This README was used primarily in the development phase and wasn't updated after development finished.
