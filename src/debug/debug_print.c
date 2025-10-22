/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 13:58:13 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/04 13:58:15 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file debug_print.c
 * @brief Debug printing functions for the Cub3D project
 * 
 * This file contains functions for printing debug information
 * about textures and maps during development and testing.
 */

#include <stdio.h>
#include "../../includes/cub3d.h"

/**
 * @brief Prints all texture information for debugging purposes
 * 
 * Displays the paths of all loaded textures (NO, SO, WE, EA)
 * and the RGB values for floor and ceiling colors.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	print_textures_debug(t_cub3d *cub)
{
	printf("[Debug] Textures:\n");
	if (cub->textures.no)
		printf("NO: %s\n", cub->textures.no);
	if (cub->textures.so)
		printf("SO: %s\n", cub->textures.so);
	if (cub->textures.we)
		printf("WE: %s\n", cub->textures.we);
	if (cub->textures.ea)
		printf("EA: %s\n", cub->textures.ea);
	if (cub->is_bonus)
	{
		if (cub->textures.door_closed)
			printf("Door Closed: %s\n", cub->textures.door_closed);
		if (cub->textures.door_open)
			printf("Door Open: %s\n", cub->textures.door_open);
	}
	if (cub->textures.floor.r != -1)
		printf("Floor: %d,%d,%d\n", cub->textures.floor.r,
			cub->textures.floor.g, cub->textures.floor.b);
	if (cub->textures.ceiling.r != -1)
		printf("Ceiling: %d,%d,%d\n", cub->textures.ceiling.r,
			cub->textures.ceiling.g, cub->textures.ceiling.b);
}

/**
 * @brief Prints the parsed map for debugging purposes
 * 
 * Displays the map dimensions and each line of the map
 * with line numbers for easy debugging.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	print_map_debug(t_cub3d *cub)
{
	int	i;

	printf("[Debug] Map (%dx%d):\n", cub->map_width, cub->map_height);
	if (!cub->map)
	{
		printf("No map loaded\n");
		return ;
	}
	i = 0;
	while (i < cub->map_height)
	{
		printf("%2d: %s\n", i, cub->map[i]);
		i++;
	}
	printf("\n");
}
