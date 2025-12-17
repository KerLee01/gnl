#include "get_next_line.h"

int main(void)
{
	char *line;
	int fd;

	fd = open("test.txt", O_RDONLY);
	line = get_next_line(fd);
	while(line != NULL)
	{
		printf("line: %s", line);
		free(line);
		line = get_next_line(fd);
	}
	return 0;
}
