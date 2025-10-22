/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_map.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:27:57 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 17:27:59 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file validate_map.c
 * @brief Map validation functions for the Cub3D project
 * 
 * This file contains functions for validating map structure,
 * including flood fill algorithm to ensure proper wall enclosure.
 */

#include "../../includes/cub3d.h"
#include "../../includes/utils.h"
#include "../../includes/validation_map_utils.h"

void	find_player_position(t_cub3d *cub);

/**
 * @brief Validates that a door at (x, y) is properly enclosed by walls/spaces.
 *
 * @param cub Pointer to the main Cub3D structure.
 * @param x X coordinate of the door.
 * @param y Y coordinate of the door.
 */
static void	validate_door_enclosure(t_cub3d *cub, int x, int y)
{
	int	orientation;

	orientation = check_door_orientation(cub, x, y);
	if (orientation == 0)
	{
		if (is_valid_position(cub, x, y - 1) && cub->map[y - 1][x] != '1'
			&& cub->map[y - 1][x] != ' ')
			ft_error("Door must have a wall/space above it", cub, NULL);
		if (is_valid_position(cub, x, y + 1) && cub->map[y + 1][x] != '1'
			&& cub->map[y + 1][x] != ' ')
			ft_error("Door must have a wall/space below it", cub, NULL);
	}
	else
	{
		if (is_valid_position(cub, x - 1, y) && cub->map[y][x - 1] != '1'
			&& cub->map[y][x - 1] != ' ')
			ft_error("Door must have a wall/space to its left", cub, NULL);
		if (is_valid_position(cub, x + 1, y) && cub->map[y][x + 1] != '1'
			&& cub->map[y][x + 1] != ' ')
			ft_error("Door must have a wall/space to its right", cub, NULL);
	}
}

/**
 * @brief Iterates over the map to validate the enclosure of all doors ('D').
 *
 * @param cub Pointer to the main Cub3D structure.
 */
static void	validate_doors(t_cub3d *cub)
{
	int	y;
	int	x;

	y = 0;
	while (y < cub->map_height)
	{
		x = 0;
		while (cub->map[y][x])
		{
			if (cub->map[y][x] == 'D')
				validate_door_enclosure(cub, x, y);
			x++;
		}
		y++;
	}
}

/**
 * @brief Allocates a 2D array to track visited positions during flood fill
 * 
 * Allocates memory for a 2D array of integers initialized to zero,
 * representing whether each position in the map has been visited.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @return Pointer to the allocated 2D visited array
 */
static int	**allocate_visited(t_cub3d *cub)
{
	int	**visited;
	int	i;

	visited = (int **)malloc(sizeof(int *) * cub->map_height);
	if (!visited)
		ft_error("Memory allocation failed for map validation", cub, NULL);
	i = 0;
	while (i < cub->map_height)
	{
		visited[i] = (int *)ft_calloc(cub->map_width, sizeof(int));
		if (!visited[i])
			handle_visited_allocation_failure(visited, i, cub);
		i++;
	}
	return (visited);
}

/**
 * @brief Initiates flood fill on all step characters in the map
 * 
 * Iterates through the map and starts flood fill from each
 * step character that hasn't been visited yet.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param visited 2D array tracking visited positions
 */
static void	flood_fill_map(t_cub3d *cub, int **visited)
{
	int	y;
	int	x;

	y = 0;
	while (y < cub->map_height)
	{
		x = 0;
		while (x < (int)ft_strlen(cub->map[y]))
		{
			if (is_step_char(cub->map[y][x]) && !visited[y][x])
				flood_fill(cub, visited, x, y);
			x++;
		}
		y++;
	}
}

/**
 * @brief Validates the map structure and enclosure
 * 
 * Ensures the map is properly enclosed by walls using
 * flood fill algorithm and validates door placements.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	validate_map(t_cub3d *cub)
{
	int	**visited;
	int	i;

	if (!cub->map || cub->map_height == 0)
		ft_error("No map found", cub, NULL);
	find_player_position(cub);
	validate_doors(cub);
	visited = allocate_visited(cub);
	flood_fill_map(cub, visited);
	i = 0;
	while (i < cub->map_height)
	{
		free(visited[i]);
		i++;
	}
	free(visited);
}
