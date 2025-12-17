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
		(library->stash_length)++;
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

	buffer = NULL;
	new_stash = NULL;
	library->nl_found = my_strchr(library->stash);
	printf("length before: %d\n", library->stash_length);
	while(library->nl_found == NULL)
	{
		buffer = malloc(sizeof(*buffer) + (BUFFER_SIZE + library->stash_length + 1));
		if(!buffer)
			return NULL;
		bytes = read(library->fd, buffer + library->stash_length, BUFFER_SIZE);
		if(bytes == 0)
		{
			if(library->stash != NULL)
				return (free(buffer), library->stash);
			return(free(buffer), NULL);
		}
		if(bytes == -1)
			return(free(buffer), NULL);
		//////////////////printf("%d\n", library->stash_length);
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
		return library->stash;
	length = library->nl_found - library->stash;
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
	(library->stash_length)--;
	while(library->stash[i] != '\n')
		i++;
	if(library->stash[i] == '\n')
		library->nl_found = &library->stash[i];
	else
		library->nl_found = NULL;
	return line;
}

int ft_strlen(char *str)
{
	int i;

	i = 0;
	while(*str)
	{
		i++;
		str++;
	}
	return i;
}

char *update_stash(t_library *library)
{
	char *updated;
	char *nl_found;
	int i;

	nl_found = library->nl_found;
	if(nl_found == NULL && library->stash != NULL)
		return library->stash;
	(nl_found)++;
	if(*nl_found == '\0')
		return NULL;
	if(nl_found)
		library->stash_length = ft_strlen(nl_found);
	i = -1;
	updated = malloc(sizeof(*updated) * (library->stash_length + 1));
	if(!updated)
		return NULL;
	while(++i < library->stash_length)
		updated[i] = nl_found[i];
	updated[i] = '\0';
	i = -1;
	library->nl_found = NULL;
	while(++i < library->stash_length)
	{
		if(nl_found[i] == '\n')
		{
			library->nl_found = &nl_found[i];
			break;
		}
	}
	return updated;
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
	current_lib->stash = update_stash(current_lib);

	return line;
}

