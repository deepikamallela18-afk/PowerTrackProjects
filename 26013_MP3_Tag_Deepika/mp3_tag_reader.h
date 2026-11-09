#ifndef MP3_TAG_READER_H
#define MP3_TAG_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

// Function prototypes
void print_help(void);
int view_mp3_tags(char *filename);
int edit_mp3_tags(char *argv[], char *filename);

#endif