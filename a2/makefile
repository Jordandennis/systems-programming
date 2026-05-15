# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread -D_DEFAULT_SOURCE
LDFLAGS = -pthread -lm

# Source files and object files
SRCS = mts.c train_thread.c queue_thread.c train.c train_queue.c utils.c
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = mts

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special case for mts.c which might not have a matching header
mts.o: mts.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies
mts.o: train.h train_thread.h queue_thread.h train_queue.h utils.h
train_thread.o: train_thread.h train.h train_queue.h utils.h
queue_thread.o: queue_thread.h train.h train_queue.h utils.h
train.o: train.h
train_queue.o: train_queue.h train.h
utils.o: utils.h

# Clean up
clean:
	rm -f $(TARGET) $(OBJS) *~

# Phony targets
.PHONY: all clean