/**
 * @file read_line.c
 * @brief Line reading utility for the Cub3D project
 * 
 * This file contains functions for reading lines from file descriptors
 * with dynamic memory allocation and proper buffer management.
 */

#include <stdlib.h>
#include <unistd.h>
#include "../../includes/utils.h"

/**
 * @brief Resizes the line buffer when capacity is exceeded
 * 
 * @param line Current line buffer
 * @param cap Pointer to current capacity (updated)
 * @param len Current length of the line
 * @return New resized buffer or NULL on failure
 */
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

/**
 * @brief Finalizes the line by null-terminating and handling EOF
 * 
 * @param line The line buffer to finalize
 * @param n Number of bytes read (0 indicates EOF)
 * @param len Current length of the line
 * @return Finalized line or NULL if EOF with empty line
 */
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

/**
 * @brief Initializes the line buffer with default capacity
 * 
 * @param line Pointer to line buffer pointer
 * @param cap Pointer to capacity variable
 * @param len Pointer to length variable
 * @return 1 on success, 0 on failure
 */
static int	init_line_buffer(char **line, int *cap, int *len)
{
	*cap = 128;
	*len = 0;
	*line = (char *)malloc(*cap);
	if (!*line)
		return (0);
	return (1);
}

/**
 * @brief Reads a complete line from a file descriptor
 * 
 * Reads characters until a newline is encountered or EOF is reached.
 * Dynamically allocates and resizes the buffer as needed.
 * 
 * @param fd File descriptor to read from
 * @return Allocated string containing the line, or NULL on EOF/error
 */
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
