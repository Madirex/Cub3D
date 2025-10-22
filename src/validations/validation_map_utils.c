/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_map_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 04:34:59 by anmateo-          #+#    #+#             */
/*   Updated: 2025/10/22 04:35:57 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d.h"
#include "../../includes/utils.h"

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
void	flood_fill(t_cub3d *cub, int **visited, int x, int y)
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
void	handle_visited_allocation_failure(
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
int	is_valid_position(t_cub3d *cub, int x, int y)
{
	if (y < 0 || y >= cub->map_height || x < 0)
		return (0);
	if (x >= (int)ft_strlen(cub->map[y]))
		return (0);
	return (1);
}

/**
 * @brief Determines the orientation of a door at (x, y).
 *
 * @param cub Pointer to the main Cub3D structure.
 * @param x X coordinate of the door.
 * @param y Y coordinate of the door.
 * @return 0 for horizontal, 1 for vertical.
 */
int	check_door_orientation(t_cub3d *cub, int x, int y)
{
	int	is_horizontal;
	int	is_vertical;

	is_horizontal = (is_valid_position(cub, x - 1, y)
			&& ft_strchr("0NSEW ", cub->map[y][x - 1]))
		&& (is_valid_position(cub, x + 1, y)
			&& ft_strchr("0NSEW ", cub->map[y][x + 1]));
	is_vertical = (is_valid_position(cub, x, y - 1)
			&& ft_strchr("0NSEW ", cub->map[y - 1][x]))
		&& (is_valid_position(cub, x, y + 1)
			&& ft_strchr("0NSEW ", cub->map[y + 1][x]));
	if (!(is_horizontal ^ is_vertical))
		ft_error("Door is not properly enclosed by walls", cub, NULL);
	if (is_horizontal)
		return (0);
	else
		return (1);
}
