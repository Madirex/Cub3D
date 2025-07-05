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

int	is_valid_map_char(char c)
{
	return (c == '0' || c == '1' || c == 'N' || c == 'S' || 
			c == 'E' || c == 'W' || c == ' ' || c == '\t');
}

int	is_map_line(const char *line)
{
	int	i;
	int	has_valid_char;

	if (!line || !*line)
		return (0);
	i = 0;
	has_valid_char = 0;
	while (line[i])
	{
		if (!is_valid_map_char(line[i]) && line[i] != '\n')
			return (0);
		if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
			has_valid_char = 1;
		i++;
	}
	return (has_valid_char);
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
			break;
		if (c == '\r')
			continue;
		line[len++] = c;
		if (len + 1 >= cap)
		{
			cap *= 2;
			char *tmp = (char *)malloc(cap);
			if (!tmp)
			{
				free(line);
				return (NULL);
			}
			ft_memcpy(tmp, line, len);
			free(line);
			line = tmp;
		}
	}
	line[len] = '\0';
	if (n == 0 && len == 0)
	{
		free(line);
		return (NULL);
	}
	return (line);
}

void	assign_map(t_cub3d *cub, char *filename)
{
	int		fd;
	char	*line;
	char	**temp_map;
	int		map_lines;
	int		map_capacity;
	int		reading_map;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		ft_error("File could not be opened", cub, NULL);
	map_capacity = 10;
	map_lines = 0;
	reading_map = 0;
	cub->map_width = 0;
	temp_map = (char **)malloc(sizeof(char *) * map_capacity);
	if (!temp_map)
		ft_error("Memory allocation failed for map", cub, NULL);
	while ((line = read_line(fd)) != NULL)
	{
		if (reading_map && (ft_strlen(line) == 0))
		{
			free(line);
			break;
		}
		if (is_map_line(line))
		{
			reading_map = 1;
			if (map_lines >= map_capacity)
			{
				map_capacity *= 2;
				char **new_temp = (char **)malloc(sizeof(char *) * map_capacity);
				if (!new_temp)
				{
					free_map(temp_map, map_lines);
					ft_error("Memory allocation failed for map expansion", cub, line);
				}
				int i = 0;
				while (i < map_lines)
				{
					new_temp[i] = temp_map[i];
					i++;
				}
				free(temp_map);
				temp_map = new_temp;
			}
			temp_map[map_lines] = ft_strdup(line);
			if (!temp_map[map_lines])
			{
				free_map(temp_map, map_lines);
				ft_error("Memory allocation failed for map line", cub, line);
			}
			if ((int)ft_strlen(line) > cub->map_width)
				cub->map_width = ft_strlen(line);
			map_lines++;
		}
		else if (reading_map && ft_strlen(line) > 0)
			ft_error("Invalid character found in map", cub, line);
		free(line);
	}
	close(fd);
	if (map_lines == 0)
	{
		free(temp_map);
		ft_error("No valid map found in the file", cub, NULL);
	}
	cub->map = temp_map;
	cub->map_height = map_lines;
}
