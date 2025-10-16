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

/**
 * @file map_utils.c
 * @brief Map utility functions for the Cub3D project
 * 
 * This file contains utility functions for map handling,
 * including memory management and character validation.
 */

#include <stdlib.h>
#include <stdio.h>
#include "../../includes/cub3d.h"
#include "../../includes/map_utils.h"

/**
 * @brief Checks if a character is valid for map content
 * 
 * @param c Character to validate
 * @param cub Pointer to the main cub3d structure (for bonus check)
 * @return 1 if character is valid for maps, 0 otherwise
 */
static int	is_valid_map_char(char c, t_cub3d *cub)
{
	if (c == '0' || c == '1' || c == 'N' || c == 'S'
        || c == 'E' || c == 'W' || c == ' ' || c == '\t')
        return (1);
        
    if (cub->is_bonus && (c == 'D' || c == 'd'))
        return (1);

    return (0);
}

/**
 * @brief Frees all allocated texture memory
 * 
 * Safely frees all texture path strings in the textures structure.
 * 
 * @param textures Pointer to the textures structure
 */
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

/**
 * @brief Frees the allocated map memory
 * 
 * Frees each line of the map and then the map array itself.
 * 
 * @param map Double pointer to the map array
 * @param height Height of the map (number of lines)
 */
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

/**
 * @brief Checks if a line contains valid map data
 * 
 * Validates that the line contains only valid map characters
 * and has at least one non-whitespace character.
 * 
 * @param line The line to check
 * @param cub Pointer to the main cub3d structure (for bonus check)
 * @return 1 if it's a valid map line, 0 otherwise
 */
int	is_map_line(const char *line, t_cub3d *cub)
{
	int	i;
	int	has_valid_char;

	if (!line || !*line)
		return (0);
	i = 0;
	has_valid_char = 0;
	while (line[i])
	{
		if (!is_valid_map_char(line[i], cub) && line[i] != '\n')
			return (0);
		if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
			has_valid_char = 1;
		i++;
	}
	return (has_valid_char);
}
