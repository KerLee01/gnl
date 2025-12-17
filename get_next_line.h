#ifndef GET_NEXT_LINE_H
#define GET_NEXT_LINE_H

#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct s_library
{
   int fd;
   int stash_length;
   char *stash;
   char *nl_found;
   char *eos;
   struct s_library *next;
} t_library;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1
#endif

char *get_next_line(int fd);
char *my_strchr(char *line);
#endif
