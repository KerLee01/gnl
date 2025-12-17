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
   char *updated_start;
   struct s_library *next;
} t_library;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 10
#endif

char *my_strchr(char *line);
char *allocate_buffer(char *buffer, t_library *library, int *buffer_length);
void free_node(t_library **library, t_library *to_remove);
void update_stash(t_library **all_nodes, t_library *library);
char *read_more(t_library *library);
t_library *find_library(int fd, t_library **library);
char *find_line(t_library *library);
char *get_next_line(int fd);

#endif
