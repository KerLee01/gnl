#include "get_next_line.h"

int main(void)
{
	char *line;
	int fd;
	int i;

	i = 0;
	fd = open("test.txt", O_RDONLY);
	line = get_next_line(fd);
	while(line != NULL)
	{
		printf("line: %s\n", line);
		free(line);
		line = get_next_line(fd);
		i++;
	}
	return 0;
}
