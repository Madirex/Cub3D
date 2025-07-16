/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_player.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anmateo- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 18:16:58 by anmateo-          #+#    #+#             */
/*   Updated: 2025/07/09 18:16:58 by anmateo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file parse_player.c
 * @brief Player parsing functions for the Cub3D project
 * 
 * This file contains functions for finding and validating
 * the player's starting position and direction in the map.
 */

#include "../../includes/cub3d.h"
#include "../../includes/utils.h"

/**
 * @brief Checks if a character represents a player
 * 
 * @param c Character to check
 * @return 1 if character is N, S, E, or W, 0 otherwise
 */
static int	is_player_char(char c)
{
	return (c == 'N' || c == 'S' || c == 'E' || c == 'W');
}

/**
 * @brief Finds the player's starting position and direction in the map
 * 
 * Scans the entire map to find exactly one player character
 * and stores the position and direction in the Cub3D structure.
 * 
 * @param cub Pointer to the main Cub3D structure
 */
void	find_player_position(t_cub3d *cub)
{
	int	i;
	int	j;
	int	player_count;

	player_count = 0;
	i = 0;
	while (i < cub->map_height)
	{
		j = 0;
		while (j < (int)ft_strlen(cub->map[i]))
		{
			if (is_player_char(cub->map[i][j]))
			{
				cub->player_x = j;
				cub->player_y = i;
				cub->player_dir = cub->map[i][j];
				player_count++;
			}
			j++;
		}
		i++;
	}
	if (player_count != 1)
		ft_error("Map must contain exactly one player", cub, NULL);
}
