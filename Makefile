# Define compiler and flags
CC=gcc
CFLAGS=-Wall -Wextra -pedantic

# Define targets
all: test

# Build the matrix object file
matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c -o matrix.o

# Build the test executable
test: matrix_test.c matrix.o
	$(CC) $(CFLAGS) matrix_test.c matrix.o -o test

# Clean up binary files
clean:
	rm -f matrix.o test
