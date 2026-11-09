CC = gcc
CFLAGS = -Wall -g

mp3tag: main.c mp3view.c mp3edit.c
	$(CC) $(CFLAGS) main.c mp3view.c mp3edit.c -o mp3tag

clean:
	rm -f mp3tag *.o