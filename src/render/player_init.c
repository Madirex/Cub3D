/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migonzal <migonzal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:30:18 by migonzal          #+#    #+#             */
/*   Updated: 2025/10/22 16:14:48 by migonzal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cub3d_render.h"
#include <string.h>
#include <math.h>

/*
 * Scan the map and return the number of player chars found.
 * If a player char is found, last found position/direction are stored
 * via out_x/out_y/out_dir.
 */
static int	scan_players(t_cub3d *cub, double *out_x,
	double *out_y, char *out_dir)
{
	int	y;
	int	x;
	int	count;

	count = 0;
	y = 0;
	while (y < cub->map_height)
	{
		x = 0;
		while (x < (int)strlen(cub->map[y]))
		{
			if (cub->map[y][x] == 'N' || cub->map[y][x] == 'S'
				|| cub->map[y][x] == 'E' || cub->map[y][x] == 'W')
			{
				*out_x = (double)x + 0.5;
				*out_y = (double)y + 0.5;
				*out_dir = cub->map[y][x];
				count++;
			}
			x++;
		}
		y++;
	}
	return (count);
}

/*
 * Public function: find player position.
 * Uses scan_players to count and obtain the coordinates.
 * If count != 1 -> ft_error(...). Otherwise set pos and direction.
 */
int	find_player_position(t_cub3d *cub)
{
	double	px;
	double	py;
	char	dir;
	int		count;

	count = scan_players(cub, &px, &py, &dir);
	if (count != 1)
		ft_error("Map must contain exactly one player", cub, NULL);
	cub->pos_x = px;
	cub->pos_y = py;
	set_player_dir(cub, dir);
	return (1);
}

/* Public init wrapper */
void	init_player(t_cub3d *cub)
{
	(void)find_player_position;
	find_player_position(cub);
}

/* Rotate player by angle (used by mouse handler) */
void	rotate_player(t_cub3d *cub, double angle)
{
	double	old_dir_x;
	double	old_plane_x;

	old_dir_x = cub->dir_x;
	cub->dir_x = cub->dir_x * cos(angle)
		- cub->dir_y * sin(angle);
	cub->dir_y = old_dir_x * sin(angle)
		+ cub->dir_y * cos(angle);
	old_plane_x = cub->plane_x;
	cub->plane_x = cub->plane_x * cos(angle)
		- cub->plane_y * sin(angle);
	cub->plane_y = old_plane_x * sin(angle)
		+ cub->plane_y * cos(angle);
}
