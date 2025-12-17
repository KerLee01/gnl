#include "get_next_line.h"

char *read_more(t_library *library)
{
	int bytes;
	int buffer_length;
	char *buffer;

	buffer_length = library->stash_length;
	buffer = NULL;
	library->nl_found = my_strchr(library->stash);
	if(library->nl_found == NULL && library->stash_length != 0)
		buffer = library->stash;
	while(library->nl_found == NULL)
	{
		buffer = allocate_buffer(buffer, library, &buffer_length);
		if(!buffer)
			return NULL;
		bytes = read(library->fd, buffer + library->stash_length, BUFFER_SIZE);
		if(bytes == 0 && library->stash[0] != '\0')
			return (library->stash);
		if(bytes == 0 || bytes == -1)
			return(NULL);
		buffer[bytes + library->stash_length] = '\0';
		library->nl_found = my_strchr(buffer + library->stash_length);
		library->stash_length += bytes;
	}
	return library->stash;
}

t_library *find_library(int fd, t_library **library)
{
	t_library *current;

	current = *library;

	if (!library)
        return (NULL);
	while(current != NULL)
	{
		if(current->fd == fd)
			return current;
		current = current->next;
	}
	current = malloc(sizeof(t_library));
	if(!current)
		return NULL;
	current->fd = fd;
	current->stash_length = 0;
	current->stash = NULL;
	current->nl_found = NULL;
	current->updated_start = NULL;
	current->next = *library;
	*library = current;
	return current;
}

char *find_line(t_library *library)
{
	char *line;
	int length;
	int i;

	if(library->nl_found == NULL)
		length = library->stash_length;
	else
		length = (library->nl_found - library->stash) + 1;
	line = malloc(sizeof(*line) * (length + 1));
	if(!line)
		return NULL;
	i = -1;
	while(++i < length)
	{
		line[i] = library->stash[i];
		(library->stash_length)--;
	}
	line[i] = '\0';
	if(library->stash[i] != '\0')
		library->updated_start = &library->stash[i];
	return line;
}

char *get_next_line(int fd)
{
	static t_library *library = NULL;
	t_library *current_lib;
	char *current_stash;
	char *line;

	if(fd < 0 || BUFFER_SIZE <= 0)
		return NULL;
	current_lib = find_library(fd, &library);
	if(current_lib == NULL)
		return NULL;
	current_stash = read_more(current_lib);
	if(current_stash == NULL)
		return (free_node(&library, current_lib), NULL);
	line = find_line(current_lib);
	if(!line)
		return(free_node(&library, current_lib), NULL);
	update_stash(&library, current_lib);
	return line;
}

