# Systems Programming in C

A collection of four low-level systems projects written in C, completed as part of CSC 360 (Operating Systems) at the University of Victoria. Each project targets a core OS concept: process management, concurrency, CPU scheduling, and memory management.

---

## Projects

### 1. Process Management & Pipes — `fetch-info` and `pipe4`

Two utilities that explore how a Unix process interacts with the operating system.

**`fetch_info`** uses `fork()` and `exec()` to spawn child processes and gather system information via Linux commands. Called without arguments it reports OS version, CPU model, available storage, and system uptime. Called with a process ID it reports information about that specific process.

**`pipe4`** implements a four-stage command pipeline from scratch. The user provides up to four shell commands interactively; the program connects them with `pipe()` and `dup2()` so that each command's stdout feeds into the next command's stdin — replicating the behaviour of the shell `|` operator at the C level.

**Concepts:** `fork()`, `exec()`, `pipe()`, `dup2()`, file descriptors, process trees

```bash
cd fetch-info
gcc -o fetch_info fetch_info.c
./fetch_info           # system summary
./fetch_info 1234      # info on process 1234

gcc -o pipe4 pipe4.c
./pipe4                # enter commands interactively
```

---

### 2. Multi-Threaded Train Station Simulator — `mts`

Simulates a single-track train station where east-bound and west-bound trains compete for access to a shared track. Each train runs as its own POSIX thread, loading for a configurable duration before queuing for the track.

The dispatcher uses mutexes and condition variables to manage track access, and implements a priority system with an anti-starvation rule: if trains of the same direction have crossed consecutively, the dispatcher yields to the opposite direction to prevent indefinite blocking.

**Concepts:** `pthread_create`, `pthread_mutex_t`, `pthread_cond_t`, condition signalling, priority queues, deadlock avoidance, race condition prevention

```bash
cd mts
gcc -o mts mts.c queue_thread.c train.c train_queue.c train_thread.c utils.c -lpthread
./mts input.txt
```

Input file format — each line is a train:
```
e:5    # east-bound, 5 second loading time
W:3    # west-bound high-priority, 3 second loading time
```

---

### 3. Multi-Level Feedback Queue Scheduler — `feedbackq`

A CPU scheduler simulator implementing the classic Multi-Level Feedback Queue (MLFQ) algorithm with three priority levels. Each level uses Round Robin scheduling with increasing time quanta (2, 4, and 8 ticks respectively). A periodic boost mechanism returns all tasks to the highest-priority queue to prevent starvation of long-running processes.

Takes a text file describing task arrival times and CPU bursts as input, and outputs a tick-by-tick execution trace.

**Concepts:** MLFQ, Round Robin scheduling, time quanta, priority demotion, starvation prevention, queue data structures

```bash
cd feedbackq
make schedule
./schedule input.txt
```

Input file format — each line is a scheduler event:
```
<tick>,<task_id>,<burst_time>
0,1,0       # task 1 arrives at tick 0
2,1,5       # task 1 requests 5-tick burst at tick 2
10,1,-1     # task 1 terminates at tick 10
```

---

### 4. Virtual Memory Simulator — `virtmem`

Simulates a hardware page table and evaluates four page replacement algorithms against a stream of memory references. Reports page faults, swap-ins, and swap-outs for each algorithm, making it straightforward to compare their efficiency on a given reference string.

**Algorithms implemented:**
- **FIFO** — evicts the page that has been in memory the longest
- **LRU** — evicts the least recently used page, tracked per-access with a timestamp counter
- **CLOCK** — approximates LRU with a reference bit and a circular hand; cheaper than true LRU
- **OPTIMAL** — evicts the page whose next use is furthest in the future (theoretical best case, requires full lookahead)

**Concepts:** virtual address translation, page tables, dirty bits, page faults, thrashing, replacement policy tradeoffs

```bash
cd virtmem
gcc -o virtmem virtmem.c
./virtmem <num_frames> <replacement_algorithm> <input_file>
# replacement_algorithm: 0=none, 1=FIFO, 2=LRU, 3=CLOCK, 4=OPTIMAL
```

---

## Building

Each project compiles with GCC on Linux. No external dependencies beyond the C standard library and pthreads.

```bash
# General pattern
gcc -Wall -o <output> <source_files> [-lpthread]
```

Tested on Ubuntu 22.04 / 24.04.

---

## Notes

These projects were completed as coursework at the University of Victoria (CSC 360, Spring 2025). Starter scaffolding for `virtmem` was provided by the course instructor; all algorithm implementations are my own work.
