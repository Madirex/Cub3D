/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:27:44 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 20:05:54 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "../../includes/cub3d.h"
#include "../../includes/map_utils.h"

static int	is_valid_map_char(char c)
{
	return (c == '0' || c == '1' || c == 'N' || c == 'S'
		|| c == 'E' || c == 'W' || c == ' ' || c == '\t');
}

void	free_textures(t_textures *textures)
{
	if (textures->no)
		free(textures->no);
	if (textures->so)
		free(textures->so);
	if (textures->we)
		free(textures->we);
	if (textures->ea)
		free(textures->ea);
}

void	free_map(char **map, int height)
{
	int	i;

	if (!map)
		return ;
	i = 0;
	while (i < height)
	{
		if (map[i])
			free(map[i]);
		i++;
	}
	free(map);
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
