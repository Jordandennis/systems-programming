schedule: feedbackq.o queue.o
	gcc feedbackq.o queue.o -o schedule

feedbackq.o: feedbackq.c queue.h
	gcc -c feedbackq.c -Wall -o feedbackq.o

queue.o: queue.c queue.h
	gcc -c queue.c -Wall -o queue.o