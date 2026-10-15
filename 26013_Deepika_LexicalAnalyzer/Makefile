CC = gcc
CFLAGS = -Wall -g

# Default target to build the executable
all: lexer

lexer: main.o lexer.o
	$(CC) $(CFLAGS) main.o lexer.o -o lexer

main.o: main.c lexer.h
	$(CC) $(CFLAGS) -c main.c

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

# Clean up compiled object files and executable
clean:
	rm -f *.o lexer