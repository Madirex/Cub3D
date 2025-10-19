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

void	find_player_position(t_cub3d *cub);

/**
 * @brief Checks if a position is at the border of the map
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param x X coordinate to check
 * @param y Y coordinate to check
 * @return 1 if position is at border, 0 otherwise
 */
static int	is_border_position(t_cub3d *cub, int x, int y)
{
	return (y == 0 || y == cub->map_height - 1
		|| x == 0 || x >= (int)ft_strlen(cub->map[y]) - 1);
}

/**
 * @brief Recursive flood fill algorithm to validate map enclosure
 * 
 * Uses flood fill to check that all accessible areas are properly
 * enclosed by walls, ensuring no open borders exist.
 * 
 * @param cub Pointer to the main Cub3D structure
 * @param visited 2D array tracking visited positions
 * @param x Current X coordinate
 * @param y Current Y coordinate
 */
static void	flood_fill(t_cub3d *cub, int **visited, int x, int y)
{
	if (y < 0 || y >= cub->map_height || x < 0)
		return ;
	if (x >= (int)ft_strlen(cub->map[y]))
		return ;
	if (visited[y][x] || cub->map[y][x] == '1')
		return ;
	if (cub->map[y][x] == ' ')
	{
		if (is_border_position(cub, x, y))
			return ;
	}
	if (is_border_position(cub, x, y) && cub->map[y][x]
			!= '1' && cub->map[y][x] != ' ')
		ft_error("Map is not closed by walls", cub, NULL);
	visited[y][x] = 1;
	flood_fill(cub, visited, x + 1, y);
	flood_fill(cub, visited, x - 1, y);
	flood_fill(cub, visited, x, y + 1);
	flood_fill(cub, visited, x, y - 1);
}

/**
 * @brief Handles memory allocation failure for visited array
 * 
 * Cleans up partially allocated visited array and exits with error.
 * 
 * @param visited Partially allocated visited array
 * @param i Number of successfully allocated rows
 * @param cub Pointer to the main Cub3D structure
 */
static void	handle_visited_allocation_failure(
				int **visited, int i, t_cub3d *cub)
{
	while (--i >= 0)
		free(visited[i]);
	free(visited);
	ft_error("Memory allocation failed for map validation", cub, NULL);
}

/**
 * @brief Checks if a given position (x, y) is within the map boundaries.
 *
 * @param cub Pointer to the main Cub3D structure.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return 1 if inside, 0 otherwise.
 */
static int	is_valid_position(t_cub3d *cub, int x, int y)
{
	if (y < 0 || y >= cub->map_height || x < 0)
		return (0);
	if (x >= (int)ft_strlen(cub->map[y]))
		return (0);
	return (1);
}

/**
 * @brief Validates if a door ('D') at (x, y) is correctly enclosed.
 *
 * A door must be surrounded by '1' (walls) or map boundaries,
 * allowing '0', 'N', 'S', 'E', 'W', or ' '
 * only on two opposing sides (horizontally or vertically).
 *
 * @param cub Pointer to the main Cub3D structure.
 * @param x X coordinate of the door.
 * @param y Y coordinate of the door.
 */
static void	validate_door_enclosure(t_cub3d *cub, int x, int y)
{
	int	is_horizontal_gap;
	int	is_vertical_gap;

	is_horizontal_gap = (is_valid_position(cub, x - 1, y)
			&& ft_strchr("0NSEW ", cub->map[y][x - 1]))
		&& (is_valid_position(cub, x + 1, y)
			&& ft_strchr("0NSEW ", cub->map[y][x + 1]));
	is_vertical_gap = (is_valid_position(cub, x, y - 1)
			&& ft_strchr("0NSEW ", cub->map[y - 1][x]))
		&& (is_valid_position(cub, x, y + 1)
			&& ft_strchr("0NSEW ", cub->map[y + 1][x]));
	if (!(is_horizontal_gap ^ is_vertical_gap))
		ft_error("Door is not properly enclosed by walls", cub, NULL);
	if (is_horizontal_gap)
	{
		if (is_valid_position(cub, x, y - 1) && cub->map[y - 1][x] != '1'
			&& cub->map[y - 1][x] != ' ')
			ft_error(
				"Door must have a wall/space above it for horizontal opening",
				cub, NULL);
		if (is_valid_position(cub, x, y + 1) && cub->map[y + 1][x] != '1'
			&& cub->map[y + 1][x] != ' ')
			ft_error(
				"Door must have a wall/space below it for horizontal opening",
				cub, NULL);
	}
	else if (is_vertical_gap)
	{
		if (is_valid_position(cub, x - 1, y) && cub->map[y][x - 1] != '1'
			&& cub->map[y][x - 1] != ' ')
			ft_error(
				"Door must have a wall/space to its left for vertical opening",
				cub, NULL);
		if (is_valid_position(cub, x + 1, y) && cub->map[y][x + 1] != '1'
			&& cub->map[y][x + 1] != ' ')
			ft_error(
				"Door must have a wall/soace to its right for vertical opening",
				cub, NULL);
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
 * @brief Main function to validate the parsed map
 * 
 * Validates that the map exists, finds the player position,
 * and uses flood fill to ensure the map is properly enclosed by walls.
 * 
 * @param cub Pointer to the main Cub3D structure containing the map
 */
void	validate_map(t_cub3d *cub)
{
	int	**visited;
	int	i;

	if (!cub->map || cub->map_height == 0)
		ft_error("No map found", cub, NULL);
	find_player_position(cub);
	validate_doors(cub);
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
	flood_fill(cub, visited, cub->player_x, cub->player_y);
	i = 0;
	while (i < cub->map_height)
	{
		free(visited[i]);
		i++;
	}
	free(visited);
}
