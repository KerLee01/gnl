#include "get_next_line.h"

char *my_strchr(char *line)
{
	if(line == NULL)
		return NULL;

	while(*line)
	{
		if(*line == '\n')
			return (line);
		line++;
	}
	return NULL;
}

char *insert_stash_buffer(t_library *library, char **buffer)
{
	int i;

	i = 0;
	while(i < library->stash_length)
	{
		(*buffer)[i] = library->stash[i];
		i++;
	}
	while((*buffer)[i] != '\0')
	{
		library->stash_length++;
		i++;
	}
	free(library->stash);
	library->eos = (*buffer) + library->stash_length;
	return (*buffer);
}

char *read_more(t_library *library)
{
	int bytes;
	char *buffer;
	char *new_stash;

	// search if stash has newline
	library->nl_found = my_strchr(library->stash);
	while(library->nl_found == NULL)
	{
		// allocate buffer size and stash_length to fit stash and amount read
		buffer = malloc(sizeof(*buffer) + (BUFFER_SIZE + library->stash_length + 1));
		if(!buffer)
			return NULL;
		// read into buffer, buffer size but start from where stash ends
		bytes = read(library->fd, buffer + library->stash_length, BUFFER_SIZE);
		// if bytes is 0, could be empty file or nothing left to read in file.
		// if the latter, stash might be filled
		if(bytes == 0)
		{
			if(library->stash != NULL)
				return (free(buffer), library->stash);
			return(free(buffer), NULL);
		}
		if(bytes == -1)
			return(free(buffer), NULL);
		buffer[bytes + library->stash_length] = '\0';
		new_stash = insert_stash_buffer(library, &buffer);
		library->nl_found = my_strchr(new_stash);
		library->stash = new_stash;
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
	current->eos = NULL;
	current->updated_start = NULL;
	current->next = *library;
	*library = current;
	return current;
}

void free_node(t_library **library, t_library *to_remove)
{
	t_library *current;
	t_library * prev;

	current = *library;
	prev = NULL;
	if(current == to_remove)
	{
		*library = current->next;
		free(to_remove->stash);
		free(to_remove);
		return;
	}
	while(current)
	{
		prev = current;
		current = current->next;
		if(current == to_remove)
		{
			prev->next = current->next;
			free(current->stash);
			free(current);
		}
	}
}

char *find_line(t_library *library)
{
	char *line;
	int length;
	int i;

	if(library->nl_found == NULL)
		length = (library->eos - library->stash);
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

void update_stash(t_library *library)
{
	char *updated;
	int i;

	i = -1;
	if(library->updated_start == NULL)
	{
		library->nl_found = NULL;
		library->eos = NULL;
		free(library->stash);
		library->stash = NULL;
		return;
	}
	updated = malloc(sizeof(*updated) * (library->eos - library->updated_start) + 1);
	while(library->updated_start[++i])
		updated[i] = library->updated_start[i];
	updated[i] = '\0';
	library->nl_found = NULL;
	library->eos = NULL;
	library->updated_start = NULL;
	free(library->stash);
	library->stash = updated;
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
	update_stash(current_lib);

	return line;
}

