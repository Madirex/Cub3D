/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:35 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:36 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../includes/cub3d.h"
#include "../../includes/parse_map.h"
#include "../../includes/map_utils.h"
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

static char	*read_line(int fd)
{
	char	*line;
	char	c;
	int		len;
	int		cap;
	int		n;

	cap = 128;
	len = 0;
	line = (char *)malloc(cap);
	if (!line)
		return (NULL);
	while ((n = read(fd, &c, 1)) > 0)
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
	}
	return (finalize_line(line, n, len));
}

static void	expand_temp_map_if_needed(t_cub3d *cub,
		char ***temp_map, int *map_lines, int *map_capacity, char *line)
{
	char **new_temp;

	if (*map_lines >= *map_capacity)
	{
		*map_capacity *= 2;
		new_temp = (char **)malloc(sizeof(char *) * (*map_capacity));
		if (!new_temp)
		{
			free_map(*temp_map, *map_lines);
			ft_error("Memory allocation failed for map expansion", cub, line);
		}
		int i = 0;
		while (i < *map_lines)
		{
			new_temp[i] = (*temp_map)[i];
			i++;
		}
		free(*temp_map);
		*temp_map = new_temp;
	}
}

static char	*trim_map_line_or_exit(t_cub3d *cub,
		char **temp_map, int map_lines, char *line)
{
	char	*trimmed_line;

	trimmed_line = ft_strtrim(line, " \t\n\r");
	if (!trimmed_line)
	{
		free_map(temp_map, map_lines);
		ft_error("Memory allocation failed for map line trimming", cub, line);
	}
	return (trimmed_line);
}

static void	update_map_width(t_cub3d *cub, const char *trimmed_line)
{
	int	len;

	len = (int)ft_strlen(trimmed_line);
	if (len > cub->map_width)
		cub->map_width = len;
}

static void	handle_map_line(t_cub3d *cub, char ***temp_map,
		int *map_lines, int *map_capacity, char *line)
{
	char	*trimmed_line;

	expand_temp_map_if_needed(cub, temp_map, map_lines, map_capacity, line);
	trimmed_line = trim_map_line_or_exit(cub,
			*temp_map, *map_lines, line);
	(*temp_map)[*map_lines] = trimmed_line;
	update_map_width(cub, trimmed_line);
	(*map_lines)++;
}

static void	finalize_map(t_cub3d *cub, char **temp_map, int map_lines)
{
	if (map_lines == 0)
	{
		free(temp_map);
		ft_error("No valid map found in the file", cub, NULL);
	}
	cub->map = temp_map;
	cub->map_height = map_lines;
}

static void	check_no_content_after_map(t_cub3d *cub,
				int fd, char **temp_map, int map_lines, char *line)
{
	char	*next_line;

	while ((next_line = read_line(fd)) != NULL)
	{
		if (!is_whitespace_only(next_line))
		{
			free(line);
			free(next_line);
			free_map(temp_map, map_lines);
			ft_error(
				"Content found after map ended. Map must be continuous.",
				cub,
				NULL);
		}
		free(next_line);
	}
	free(line);
}

static void	init_map_vars(t_cub3d *cub, char ***temp_map,
		int *map_capacity, int *map_lines)
{
	*map_lines = 0;
	cub->map_width = 0;
	*map_capacity = 5;
	*temp_map = (char **)malloc(sizeof(char *) * (*map_capacity));
	if (!*temp_map)
		ft_error("Memory allocation failed for map", cub, NULL);
}

static int	open_map_file(const char *filename, t_cub3d *cub)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		ft_error("File could not be opened", cub, NULL);
	return (fd);
}

static void	process_map_lines(t_cub3d *cub, int fd,
		char ***temp_map, int *map_lines, int *map_capacity)
{
	int		reading_map;
	char	*line;

	reading_map = 0;
	while ((line = read_line(fd)) != NULL)
	{
		if (reading_map && is_whitespace_only(line))
		{
			check_no_content_after_map(cub, fd, *temp_map, *map_lines, line);
			break ;
		}
		if (is_map_line(line))
		{
			reading_map = 1;
			handle_map_line(cub, temp_map, map_lines, map_capacity, line);
		}
		else if (reading_map && !is_whitespace_only(line))
			ft_error("Invalid character found in map", cub, line);
		free(line);
	}
}

void	assign_map(t_cub3d *cub, char *filename)
{
	int		map_capacity;
	int		map_lines;
	char	**temp_map;
	int		fd;

	init_map_vars(cub, &temp_map, &map_capacity, &map_lines);
	fd = open_map_file(filename, cub);
	process_map_lines(cub, fd, &temp_map, &map_lines, &map_capacity);
	close(fd);
	finalize_map(cub, temp_map, map_lines);
}
