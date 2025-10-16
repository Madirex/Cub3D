/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:48 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 19:59:14 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file utils.c
 * @brief General utility functions for the Cub3D project
 * 
 * This file contains general-purpose utility functions including
 * error handling, initialization, and string validation.
 */

#include <stdlib.h>
#include <stdio.h>
#include "../../includes/cub3d.h"
#include "../../includes/map_utils.h"

/**
 * @brief Safely exits the program after cleaning up resources
 * 
 * @param cub Pointer to the main Cub3D structure (for cleanup)
 * @param line Line to free if not NULL
 */
static void	safe_exit(t_cub3d *cub, char *line)
{
	if (cub)
	{
		free_textures(&cub->textures);
		if (cub->map)
			free_map(cub->map, cub->map_height);
	}
	if (line)
		free(line);
	exit(1);
}

/**
 * @brief Handles errors by printing a message and cleaning up resources
 * 
 * Prints "Error" followed by the provided message, then safely
 * exits the program after cleaning up allocated resources.
 * 
 * @param message Error message to display
 * @param cub Pointer to the main Cub3D structure (for cleanup)
 * @param line Line to free if not NULL
 */
void	ft_error(char *message, t_cub3d *cub, char *line)
{
	printf("Error\n");
	if (message)
		printf("%s\n", message);
	safe_exit(cub, line);
}

/**
 * @brief Checks if a line contains only whitespace characters
 * 
 * @param line The line to check
 * @return 1 if line contains only whitespace or is NULL, 0 otherwise
 */
int	is_whitespace_only(const char *line)
{
	int	i;

	if (!line)
		return (1);
	i = 0;
	while (line[i])
	{
		if (line[i] != ' ' && line[i] != '\t'
			&& line[i] != '\n' && line[i] != '\r')
			return (0);
		i++;
	}
	return (1);
}

/**
 * @brief Initializes the main Cub3D structure with default values
 * 
 * Sets all fields to zero and initializes RGB color values to -1
 * to indicate they haven't been set yet.
 * 
 * @param cub Pointer to the main Cub3D structure to initialize
 * @param is_bonus Flag indicating if bonus features are enabled
 */
void	init_cub3d(t_cub3d *cub, int is_bonus)
{
	ft_memset(cub, 0, sizeof(t_cub3d));
	cub->textures.floor.r = -1;
	cub->textures.floor.g = -1;
	cub->textures.floor.b = -1;
	cub->textures.ceiling.r = -1;
	cub->textures.ceiling.g = -1;
	cub->textures.ceiling.b = -1;
	cub->is_bonus = is_bonus;
}
