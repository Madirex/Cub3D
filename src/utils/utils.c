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
#include <mlx.h>

/**
 * @brief Safely exits the program after cleaning up resources
 * 
 * @param cub Pointer to the main Cub3D structure (for cleanup)
 * @param line Line to free if not NULL
 * @param exit_code Exit code
 */
void	safe_exit(t_cub3d *cub, char *line, int exit_code)
{
	int	i;

	if (cub)
	{
		if (cub->fd >= 0)
			close(cub->fd);
		if (cub->img && cub->mlx)
			mlx_destroy_image(cub->mlx, cub->img);
		if (cub->win && cub->mlx)
			mlx_destroy_window(cub->mlx, cub->win);
		if (cub->wall_textures)
		{
			i = 0;
			while (i < 4 && cub->wall_textures[i])
				free(cub->wall_textures[i++]);
			free(cub->wall_textures);
		}
		if (cub->door_textures)
		{
			i = 0;
			while (i < 4 && cub->door_textures[i])
				free(cub->door_textures[i++]);
			free(cub->door_textures);
		}
		free_textures(&cub->textures);
		if (cub->map)
			free_map(cub->map, cub->map_height);
		if (cub->mlx)
		{
			free(cub->mlx);
			cub->mlx = NULL;
		}
	}
	if (line)
		free(line);
	exit(exit_code);
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
	write(2, "Error\n", 6);
	if (message)
	{
		write(2, message, ft_strlen(message));
		write(2, "\n", 1);
	}
	safe_exit(cub, line, 1);
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

/**
 * @brief Cleans up resources and exits the program
 * * @param cub Pointer to the main Cub3D structure
 * @return 0 (never returns, exits the program)
 */
int	exit_program(t_cub3d *cub)
{
	safe_exit(cub, NULL, 0);
	return (0);
}
