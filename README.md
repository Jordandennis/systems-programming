# Systems Programming in C — Operating Systems Concepts

Low-level systems programs written in C for an operating-systems course (CSC 360, University of Victoria), exploring process control, concurrency, CPU scheduling, and virtual memory at the level the operating system works at.

This repository is a portfolio showcase: it documents the design and the problems each program solves. It is intended to demonstrate my understanding, not to be reused.

---

## ⚠️ Authorship & Reuse Notice

© Jordan Dennis. **All rights reserved — no license is granted for reuse.**
This is my own academic work, shared publicly for portfolio and demonstration purposes only. If you are a current student: do not copy, adapt, or submit any part of this work for coursework. Doing so is academic misconduct and is solely your responsibility. This repository is published to document my own authorship and learning.

---

## What this work demonstrates

- **Process control & IPC** — creating processes and connecting them with pipes (`fork`, `exec`, `pipe`, `dup2`, `waitpid`)
- **Concurrent programming** — POSIX threads with mutexes and condition variables, and reasoning about correctness under concurrency (deadlock and starvation avoidance)
- **CPU scheduling** — implementing and comparing scheduling policies
- **Virtual memory** — page-replacement algorithms and their fault behaviour

## Components

### 1. Shell command pipeline
A minimal shell that parses a command line into a pipeline of processes and wires them together with pipes. Each stage is a forked child whose standard input/output is redirected with `dup2`, with the parent managing process creation and reaping. The interesting part is getting the file-descriptor plumbing and process lifecycle exactly right so that arbitrary-length pipelines behave like a real shell.

### 2. Multithreaded simulation
A concurrent simulation that coordinates multiple threads competing for shared resources, using POSIX threads with mutexes and condition variables. The core challenge was correctness under concurrency: ensuring mutually exclusive access to shared state while structuring the signalling so the system cannot deadlock and no thread is starved. *(Scenario modelled: [add a one-line description of what the simulation represents].)*

### 3. Multi-Level Feedback Queue (MLFQ) scheduler
An implementation of an MLFQ CPU scheduler with multiple priority queues, per-level time quanta, and feedback that moves processes between levels based on their behaviour. Demonstrates the trade-offs MLFQ makes between responsiveness for short/interactive jobs and throughput for long-running ones.

### 4. Virtual-memory simulator
Simulates page replacement under several policies — **FIFO, LRU, CLOCK, and OPTIMAL** — and compares their page-fault behaviour over the same reference stream. A good lens on why real systems approximate LRU with CLOCK rather than implementing true LRU.

## Design highlights / what I took away

- Reasoning rigorously about **race conditions** and proving to myself that a concurrent design is correct, not just that it happens to work
- The practical gap between a scheduling/replacement policy on paper and the bookkeeping needed to implement it
- Working close to the hardware in C: manual memory and resource management, and careful handling of edge cases

## Tech

C · POSIX threads (pthreads) · Linux

## A note on completeness

To keep this useful as a portfolio piece without serving as a ready-made solution for current students, the README focuses on design and reasoning. If you are an employer and would like a deeper walkthrough of any component, I am glad to talk through it directly.
