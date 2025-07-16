#include <stdlib.h>
#include <unistd.h>
#include "../../includes/utils.h"

static char	*resize_line_buffer(char *line, int *cap, int len)
{
	char	*tmp;

	*cap *= 2;
	tmp = (char *)malloc(*cap);
	if (!tmp)
	{
		free(line);
		return (NULL);
	}
	ft_memcpy(tmp, line, len);
	free(line);
	return (tmp);
}

static char	*finalize_line(char *line, int n, int len)
{
	line[len] = '\0';
	if (n == 0 && len == 0)
	{
		free(line);
		return (NULL);
	}
	return (line);
}

static int	init_line_buffer(char **line, int *cap, int *len)
{
	*cap = 128;
	*len = 0;
	*line = (char *)malloc(*cap);
	if (!*line)
		return (0);
	return (1);
}

char	*read_line(int fd)
{
	char	*line;
	char	c;
	int		len;
	int		cap;
	int		n;

	if (!init_line_buffer(&line, &cap, &len))
		return (NULL);
	n = read(fd, &c, 1);
	while (n > 0)
	{
		if (c == '\n')
			break ;
		if (c == '\r')
			continue ;
		line[len++] = c;
		if (len + 1 >= cap)
		{
			line = resize_line_buffer(line, &cap, len);
			if (!line)
				return (NULL);
		}
		n = read(fd, &c, 1);
	}
	return (finalize_line(line, n, len));
}
