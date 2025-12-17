#include "get_next_line_bonus.h"

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

char *allocate_buffer(char *buffer, t_library *library, int *buffer_length)
{
	char *decoy;
	int i;

	i = 0;
	if(*buffer_length <= BUFFER_SIZE + library->stash_length)
	{
		*buffer_length *= (BUFFER_SIZE + library->stash_length);
		if(*buffer_length <= BUFFER_SIZE + library->stash_length)
			*buffer_length = BUFFER_SIZE + library->stash_length;
		decoy = malloc(sizeof(*decoy) * (*buffer_length) + 1);
		if (!decoy)
			return NULL;
		while(buffer != NULL && buffer[i] != '\0')
		{
			decoy[i] = buffer[i];
			i++;
		}
		decoy[i] = '\0';
		free(buffer);
		buffer = decoy;
		library->stash = buffer;
	}
	return buffer;
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
			return;
		}
	}
}

void update_stash(t_library **all_nodes, t_library *library)
{
	char *updated;
	int i;

	i = -1;
	if(library->updated_start == NULL)
	{
		library->nl_found = NULL;
		free(library->stash);
		library->stash = NULL;
		return;
	}
	updated = malloc(sizeof(*updated) * (library->stash_length + 1));
	if(!updated)
	{
		free_node(all_nodes, library);
		return;
	}
	while(library->updated_start[++i] != '\0')
		updated[i] = library->updated_start[i];
	updated[i] = '\0';
	library->nl_found = NULL;
	library->updated_start = NULL;
	free(library->stash);
	library->stash = updated;
}
